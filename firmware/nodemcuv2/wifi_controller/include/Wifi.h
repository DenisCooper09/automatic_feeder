#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <LED.h>

namespace Feeder {
    class WifiConfig {
    public:
        WifiConfig(const char *ssid, const char *password);

        WifiConfig(const char *ssid, const char *password, LED &status_led);

        WifiConfig(const char *ssid, const char *password, uint8_t status_led_pin);

        WifiConfig(const char *ssid, const char *password, uint32_t timeout);

        WifiConfig(const char *ssid, const char *password, uint32_t timeout, LED &status_led);

        WifiConfig(const char *ssid, const char *password, uint32_t timeout, uint8_t status_led_pin);

        WifiConfig(WifiConfig &other);

    public:
        const char *GetSSID();

        const char *GetPassword();

        [[nodiscard]] uint32_t GetTimeout() const;

        std::shared_ptr<LED> &GetStatusLED();

    private:
        const char *_ssid, *_password;

        uint32_t _timeout = 5000;

        std::shared_ptr<LED> _status_led;
    };


    class Wifi {
    public:
        explicit Wifi(WifiConfig &config);

    public:
        void Connect();

        bool IsConnected();

        void Update();

    private:
        std::unique_ptr<WifiConfig> _config;
    };
}
