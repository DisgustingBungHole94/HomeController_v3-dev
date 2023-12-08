#pragma once

#include <fstream>
#include <memory>
#include <mutex>
#include <string>

class PWM {
    public:
        static bool init(const std::string& pwm_file);

        static void analog_write(unsigned int pin, float value);

        static void reset();
        static void release_pin(unsigned int pin);

        static void stop();

        static const unsigned int PWM_PIN_R;
        static const unsigned int PWM_PIN_G;
        static const unsigned int PWM_PIN_B;

    private:
        static bool _init;
        static std::ofstream _file;

        static std::mutex _mutex;
};