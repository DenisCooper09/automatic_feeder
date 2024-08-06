#pragma once

#include <Arduino.h>
#include <memory>

namespace Feeder {
    class LED {
    public:
        explicit LED(uint8_t pin);

        LED(LED &other);

    public:
        void Init();

        void On();

        void Off();

        void Invert();

        void Display() const;

        static void Init(std::shared_ptr<Feeder::LED> &led);

        static void On(std::shared_ptr<Feeder::LED> &led);

        static void Off(std::shared_ptr<Feeder::LED> &led);

        static void Invert(std::shared_ptr<Feeder::LED> &led);

        static void Display(std::shared_ptr<Feeder::LED> &led);

    private:
        uint8_t _pin;

        bool _state = false;
    };
}
