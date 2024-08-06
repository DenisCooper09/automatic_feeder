#include <AutomaticFeeder.h>

namespace Feeder {
    AutomaticFeeder::AutomaticFeeder() {
        WifiConfig wifi_config = WifiConfig(WIFI_SSID, WIFI_PASSWORD, WIFI_TIMEOUT, WIFI_STATUS_LED_PIN);
        _wifi = std::make_unique<Wifi>(wifi_config);
        _server = std::make_unique<AsyncWebServer>(80);

        Wire.begin();

        // TODO: Fix the issue with RTC not working.
        _rtc.begin();
        _rtc.adjust(DateTime(__DATE__, __TIME__));
    }

    void AutomaticFeeder::Initialize() {
        Serial.begin(115200);

        while (!Serial) delay(1);

        pinMode(SS, OUTPUT);
        digitalWrite(SS, HIGH);
        SPI.begin();

        if (!LittleFS.begin()) {
            Serial.println("[ERROR] Error mounting LittleFS.");
            return;
        }

        _wifi->Connect();

        if (!MDNS.begin(MDNS_HOSTNAME)) {
            Serial.println("[ERROR] Error setting up MDNS responder.");
        }

        BeginServer();

        UpdateSchedule();
    }

    void AutomaticFeeder::Update() {
        _wifi->Update();
        MDNS.update();

        /*
        static uint32_t timer = millis();
        if (millis() - timer >= 1000) {
            timer = millis();

            DateTime time = _rtc.now();

            for (const FeedingScheduleInfo info: _schedule) {
                if (info.hour == time.hour() && info.minute == time.minute()) {
                    // Feed(info.amount);
                    Serial.printf("Feeding! Amount: %i", info.amount);
                }
            }
        }
        */
    }

    void AutomaticFeeder::BeginServer() {
        _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(LittleFS, "/web/index.html", "text/html");
        });

        _server->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(LittleFS, "/web/style.css", "text/css");
        });

        _server->on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(LittleFS, "/web/script.js", "text/javascript");
        });

        _server->on(schedule_file_path, HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(LittleFS, schedule_file_path, "application/json");
        });

        _server->onNotFound([](AsyncWebServerRequest *request) {
            request->send(404, "text/plain", "404: Not found");
        });

        _server->on("/feed", HTTP_GET, [](AsyncWebServerRequest *request) {
            Feed(static_cast<uint8_t>(request->arg("amount").toInt()));
        });

        _server->on(
                "/schedule", HTTP_POST, [](AsyncWebServerRequest *request) {}, nullptr,
                [this](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
                    static String json_data;

                    if (index == 0) {
                        json_data = "";
                    }

                    json_data += String((char *) data).substring(0, len);

                    if (index + len == total) {
                        Serial.printf("JSON received:\n%s", json_data.c_str());
                        request->send(200, "text/plain", "JSON received");

                        if (LittleFS.exists(schedule_file_path)) LittleFS.remove(schedule_file_path);

                        File schedule_file = LittleFS.open(schedule_file_path, "w");
                        if (!schedule_file) {
                            Serial.println("[ERROR] Failed to open schedule JSON file for writing.");
                            return;
                        }

                        schedule_file.print(json_data);
                        schedule_file.close();

                        UpdateSchedule();

                        json_data = "";
                    }
                });

        _server->begin();
    }

    void AutomaticFeeder::UpdateSchedule() {
        _schedule.clear();

        File file = LittleFS.open(schedule_file_path, "r");
        if (!file) {
            Serial.println("[ERROR] Failed to open schedule JSON file for reading.");
            return;
        }

        String json_data = file.readString();
        file.close();

        Serial.println(json_data);

        JsonDocument document;
        deserializeJson(document, json_data);
        for (const JsonVariant &item: document.as<JsonArray>()) {
            const uint8_t amount = item["amount"];
            const uint8_t hour = item["time"]["hour"];
            const uint8_t minute = item["time"]["minute"];

            _schedule.push_back((FeedingScheduleInfo) {amount, hour, minute});

            Serial.printf("\n%i:%i Amount: %i", hour, minute, amount);
        }
    }

    void AutomaticFeeder::Feed(uint8_t amount) {
        digitalWrite(SS, LOW);
        SPI.transfer(0x1F);
        SPI.transfer(amount);
        digitalWrite(SS, HIGH);
    }
}
