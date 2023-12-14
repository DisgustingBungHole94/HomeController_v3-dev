#include "pwm.h"

#include <homecontroller/util/logger.h>

#include <pigpio.h>
#include <iostream>

const unsigned int PWM::PWM_PIN_R = 27;
const unsigned int PWM::PWM_PIN_G = 22;
const unsigned int PWM::PWM_PIN_B = 23;

bool PWM::_init = false;

bool PWM::init() {
//#ifdef __arm__
    std::cout << "test!" << std::endl;

    if (gpioInitialise() < 0) {
        return false;
    }

    static const int FREQUENCY = 1000;
    static const int DUTY_CYCLE = 255;

    bool error = false;

    // set pins to output
    /*error = (gpioSetMode(PWM_PIN_R, PI_OUTPUT) != 0); 
    error = (gpioSetMode(PWM_PIN_G, PI_OUTPUT) != 0); 
    error = (gpioSetMode(PWM_PIN_B, PI_OUTPUT) != 0);*/

    // set PWM frequency
    /*error = (gpioSetPWMfrequency(PWM_PIN_R, FREQUENCY) != 0);
    error = (gpioSetPWMfrequency(PWM_PIN_R, FREQUENCY) != 0);
    error = (gpioSetPWMfrequency(PWM_PIN_R, FREQUENCY) != 0);*/

    // set PWM duty cycle
    error = (gpioPWM(PWM_PIN_R, 0) != 0);
    error = (gpioPWM(PWM_PIN_G, 0) != 0);
    error = (gpioPWM(PWM_PIN_B, 0) != 0);

    /*error = (gpioWrite(PWM_PIN_R, 1) != 0);
    error = (gpioWrite(PWM_PIN_G, 1) != 0);
    error = (gpioWrite(PWM_PIN_B, 1) != 0);*/

    if (error) {
        gpioTerminate();
        return false;
    }

    return true;
/*#else
    return true;
#endif*/
}

void PWM::analog_write(unsigned int pin, uint8_t value) {
    if (!_init) {
        return;
    }

    gpioPWM(pin, value);
}

void PWM::reset() {
    if (!_init) {
        return;
    }

    gpioPWM(PWM_PIN_R, 0);
    gpioPWM(PWM_PIN_G, 0);
    gpioPWM(PWM_PIN_B, 0);
}

void PWM::stop() {
    if (!_init) {
        return;
    }

    gpioTerminate();
}