#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Wifi.h>
#include <Config.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <RTClib.h>

namespace Feeder {
    struct FeedingScheduleInfo {
        uint8_t amount, hour, minute;
    };

    constexpr const char *schedule_file_path = "/schedule.json";

    class AutomaticFeeder {
    public:
        AutomaticFeeder();

    public:
        void Initialize();

        void Update();

    private:
        void BeginServer();

        void UpdateSchedule();

        static void Feed(uint8_t amount);

    private:
        std::unique_ptr<Wifi> _wifi;
        std::unique_ptr<AsyncWebServer> _server;
        std::vector<FeedingScheduleInfo> _schedule;
        RTC_DS1307 _rtc;
    };
}
