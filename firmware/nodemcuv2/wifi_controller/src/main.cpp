#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <RTClib.h>
#include <SPI.h>

#define WIFI_SSID "vodafone2B40D4"
#define WIFI_PASSWORD "HKftgFF2qdrhm7Mg"
#define WIFI_TIMEOUT 5000
#define WIFI_STATUS_LED D3
#define MDNS_HOSTNAME "feeder"
#define DEBUG 1

AsyncWebServer server(80);
RTC_DS1307 rtc;

void feed(uint8_t amount) {
    digitalWrite(SS, LOW);
    SPI.transfer(0x1F);
    SPI.transfer(amount);
    Serial.print(amount);
    digitalWrite(SS, HIGH);
}

///////////////////////////////////////////////////////////////////
// WiFi
///////////////////////////////////////////////////////////////////

bool is_connected() {
    return WiFi.status() == WL_CONNECTED;
}

void connect_to_wifi(const char *SSID, const char *PASSWORD) {
#if DEBUG > 0
    Serial.print("\nConnecting to ");
    Serial.print(WIFI_SSID);
#endif

    WiFi.begin(SSID, PASSWORD);

    while (!is_connected()) {
        yield();

        static uint32_t wifi_timer = millis();
        if (millis() - wifi_timer > 250) {
            wifi_timer = millis();
            digitalWrite(WIFI_STATUS_LED, !digitalRead(WIFI_STATUS_LED));

#if DEBUG > 0
            Serial.print('.');
#endif
        }
    }

    digitalWrite(WIFI_STATUS_LED, 1);

#if DEBUG > 0
    Serial.println("\nConnected.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
#endif
}

[[maybe_unused]] void setup() {
    pinMode(WIFI_STATUS_LED, OUTPUT);
    digitalWrite(WIFI_STATUS_LED, 0);

    Serial.begin(115200);

    pinMode(SS, OUTPUT);
    digitalWrite(SS, HIGH);
    SPI.begin();

    if (!LittleFS.begin()) {
        Serial.println("An error has occurred while mounting LittleFS.");
        return;
    }

    connect_to_wifi(WIFI_SSID, WIFI_PASSWORD);

    if (!MDNS.begin(MDNS_HOSTNAME)) {
#if DEBUG > 0
        Serial.println("Error setting up MDNS responder!");
#endif
    }

    ///////////////////////////////////////////////////////////////////
    // Server requests
    ///////////////////////////////////////////////////////////////////

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html");
    });

    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/style.css", "text/css");
    });

    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/script.js", "text/javascript");
    });

    server.on("/feed", HTTP_GET, [](AsyncWebServerRequest *request) {
        const String AMOUNT_STR = request->arg("amount");
        feed((uint8_t) AMOUNT_STR.toInt());

        request->send(200, "text/plain", "Feeding action triggered with amount: " + AMOUNT_STR);
    });

    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "404: Not found");
    });

    server.begin();
}

void loop() {
    MDNS.update();

    static uint32_t wifi_timeout = millis();
    if (millis() - wifi_timeout > WIFI_TIMEOUT) {
        wifi_timeout = millis();
        if (!is_connected()) {
            digitalWrite(WIFI_STATUS_LED, 0);
            connect_to_wifi(WIFI_SSID, WIFI_PASSWORD);
        }
    }
}
