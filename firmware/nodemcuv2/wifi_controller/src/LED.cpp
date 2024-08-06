#include <LED.h>

namespace Feeder {
    LED::LED(uint8_t pin) {
        _pin = pin;
    }

    LED::LED(LED &other) {
        _pin = other._pin;
    }

    void LED::Init() {
        pinMode(_pin, OUTPUT);
        Off();
    }

    void LED::On() {
        _state = true;
        Display();
    }

    void LED::Off() {
        _state = false;
        Display();
    }

    void LED::Invert() {
        _state = !_state;
        Display();
    }

    void LED::Display() const {
        digitalWrite(_pin, _state);
    }

    void LED::Init(std::shared_ptr<Feeder::LED> &led) {
        if (led) led->Init();
    }

    void LED::On(std::shared_ptr<Feeder::LED> &led) {
        if (led) led->On();
    }

    void LED::Off(std::shared_ptr<Feeder::LED> &led) {
        if (led) led->Off();
    }

    void LED::Invert(std::shared_ptr<Feeder::LED> &led) {
        if (led) led->Invert();
    }

    void LED::Display(std::shared_ptr<Feeder::LED> &led) {
        if (led) led->Display();
    }
}
