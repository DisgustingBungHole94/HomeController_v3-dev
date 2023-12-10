#include "pwm.h"

#include <homecontroller/util/logger.h>

const unsigned int PWM::PWM_PIN_R = 27;
const unsigned int PWM::PWM_PIN_G = 22;
const unsigned int PWM::PWM_PIN_B = 23;

bool PWM::_init = false;
std::ofstream PWM::_file;

std::mutex PWM::_mutex;

bool PWM::init(const std::string& pwm_file) {
    _file.open(pwm_file);
    if (!_file.is_open()) {
        return false;
    }

    _init = true;
    return true;
}

void PWM::analog_write(unsigned int pin, float value) {
    if (!_init) return;

    std::lock_guard<std::mutex> lock(_mutex);

    //_file << std::to_string(pin) << "=" << std::to_string(value) << "\n";
    //_file.flush();

    if (!_file.good()) {
        hc::util::logger::err("failed to write to pin " + std::to_string(pin) + "!");
    }
}

void PWM::reset() {
    if (!_init) return;

    std::lock_guard<std::mutex> lock(_mutex);

    _file << "*=0" << "\n";
    _file.flush();

    if (!_file.good()) {
        hc::util::logger::err("failed to reset pins!");
    }
}

void PWM::release_pin(unsigned int pin) {
    if (!_init) return;

    std::lock_guard<std::mutex> lock(_mutex);

    _file << "release " << std::to_string(pin) << "\n";
    _file.flush();

    if (!_file.good()) {
        hc::util::logger::err("failed to release pin " + std::to_string(pin) + "!");
    }
}

void PWM::stop() {
    if (!_init) return;

    std::lock_guard<std::mutex> lock(_mutex);

    _file.close();
    if (!_file.good()) {
        hc::util::logger::err("failed to shutdown PWM!");
    }
}