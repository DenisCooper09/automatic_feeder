#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <SPI.h>

#define WIFI_SSID "wifi_network_name"
#define WIFI_PASSWORD "wifi_password"

AsyncWebServer server(80);

void feed(const uint8_t &AMOUNT) {
    digitalWrite(SS, LOW);
    SPI.transfer(1);
    SPI.transfer(AMOUNT);
    digitalWrite(SS, HIGH);
}

[[maybe_unused]] void setup() {
    Serial.begin(115200);

    pinMode(SS, OUTPUT);
    digitalWrite(SS, HIGH);
    SPI.begin();

    if (!LittleFS.begin()) {
        Serial.println("An error has occurred while mounting LittleFS.");
        return;
    }

    Serial.print("Connecting to ");
    Serial.print(WIFI_SSID);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        yield();

        static uint32_t wifi_timer = millis();
        if (millis() - wifi_timer > 500) {
            wifi_timer = millis();
            Serial.print('.');
        }
    }

    Serial.println("\nConnected.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

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
        const auto FEED_AMOUNT = static_cast<uint8_t>(AMOUNT_STR.toInt());
        feed(FEED_AMOUNT);
        request->send(200, "text/plain", "Feeding action triggered with amount: " + AMOUNT_STR);
    });

    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "404: Not found");
    });

    server.begin();
}

void loop() {}
