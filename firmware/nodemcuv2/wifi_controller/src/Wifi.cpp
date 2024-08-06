#include <Wifi.h>

namespace Feeder {
    WifiConfig::WifiConfig(const char *ssid, const char *password) {
        _ssid = ssid;
        _password = password;
        _status_led = nullptr;
    }

    WifiConfig::WifiConfig(const char *ssid, const char *password, LED &status_led) {
        _ssid = ssid;
        _password = password;
        _status_led = std::make_shared<LED>(status_led);
        _status_led->Init();
    }

    WifiConfig::WifiConfig(const char *ssid, const char *password, uint8_t status_led_pin) {
        _ssid = ssid;
        _password = password;
        _status_led = std::make_shared<LED>(status_led_pin);
        _status_led->Init();
    }

    WifiConfig::WifiConfig(const char *ssid, const char *password, uint32_t timeout) {
        _ssid = ssid;
        _password = password;
        _status_led = nullptr;
        _timeout = timeout;
    }

    WifiConfig::WifiConfig(const char *ssid, const char *password, uint32_t timeout, LED &status_led) {
        _ssid = ssid;
        _password = password;
        _status_led = std::make_shared<LED>(status_led);
        _status_led->Init();
        _timeout = timeout;
    }

    WifiConfig::WifiConfig(const char *ssid, const char *password, uint32_t timeout, uint8_t status_led_pin) {
        _ssid = ssid;
        _password = password;
        _status_led = std::make_shared<LED>(status_led_pin);
        _status_led->Init();
        _timeout = timeout;
    }

    WifiConfig::WifiConfig(WifiConfig &other) {
        _ssid = other._ssid;
        _password = other._password;
        _status_led = other._status_led;
    }

    const char *WifiConfig::GetSSID() {
        return _ssid;
    }

    const char *WifiConfig::GetPassword() {
        return _password;
    }

    uint32_t WifiConfig::GetTimeout() const {
        return _timeout;
    }

    std::shared_ptr<LED> &WifiConfig::GetStatusLED() {
        return _status_led;
    }


    Wifi::Wifi(WifiConfig &config) {
        _config = std::make_unique<WifiConfig>(config);
    }

    void Wifi::Connect() {
        WiFi.begin(_config->GetSSID(), _config->GetPassword());

        while (!IsConnected()) {
            yield();

            static uint32_t wifi_timer = millis();
            if (millis() - wifi_timer > 250) {
                wifi_timer = millis();

                LED::Invert(_config->GetStatusLED());

                Serial.print('.');
            }
        }

        LED::On(_config->GetStatusLED());

        Serial.printf("Connected. IP Address: %s", WiFi.localIP().toString().c_str());
    }

    bool Wifi::IsConnected() {
        return WiFi.isConnected() && WiFi.SSID() == _config->GetSSID();
    }

    void Wifi::Update() {
        static uint32_t wifi_timeout = millis();
        if (millis() - wifi_timeout > _config->GetTimeout()) {
            wifi_timeout = millis();
            if (!IsConnected()) {
                LED::Off(_config->GetStatusLED());

                Connect();
            }
        }
    }
}
