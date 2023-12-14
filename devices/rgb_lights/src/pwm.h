#pragma once

#include <fstream>
#include <memory>
#include <mutex>
#include <string>

class PWM {
    public:
        static bool init();

        static void analog_write(unsigned int pin, uint8_t value);

        static void reset();

        static void stop();

        static const unsigned int PWM_PIN_R;
        static const unsigned int PWM_PIN_G;
        static const unsigned int PWM_PIN_B;

    private:
        static bool _init;
};