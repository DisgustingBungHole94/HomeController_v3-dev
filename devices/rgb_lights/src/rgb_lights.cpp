#include "rgb_lights.h"

#include "programs/rainbow_fade_program.h"
#include "pwm.h"

#include <homecontroller/api/request_maker.h>
#include <homecontroller/exception.h>
#include <homecontroller/util/logger.h>

#include <unordered_map>
#include <iostream>

bool rgb_lights::start() {
    hc::util::logger::enable_debug();

    hc::util::logger::log("RGBLights v0.0.0 by Josh Dittmer");

    if (!PWM::init()) {
        hc::util::logger::csh("failed to start PWM!");
        return false;
    }

    PWM::reset();

    hc::util::logger::log("PWM started");

    m_state.set_r(0x00);
    m_state.set_g(0x00);
    m_state.set_b(0x00);

    m_state.set_speed(1.0f);

    m_state.set_program(hc::api::rgb_lights_state::program::NONE);

    hc::util::logger::log("connecting to server...");

    std::thread update_color_thread;
    bool update_color_thread_started = false;

    hc::api::request_maker api;
    try {
        api.connect();

        m_device = api.login_device("test", "1234", "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB");

        m_device.set_turn_on_callback(std::bind(&rgb_lights::turn_on_callback, this));
        m_device.set_turn_off_callback(std::bind(&rgb_lights::turn_off_callback, this));
        m_device.set_data_callback(std::bind(&rgb_lights::data_callback, this, std::placeholders::_1));
    
        m_running = true;

        m_device.run(hc::api::state::power::OFF, m_state.serialize());
    } catch(hc::exception& e) {
        hc::util::logger::err("device exception occurred: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");
        m_device.stop();
    }

    m_running = false;

    hc::util::logger::log("shutting down...");

    if (m_program_running) {
        hc::util::logger::log("stopping program...");
        stop_program();
    }

    if (api.is_connected()) {
        api.disconnect();
    }

    if (m_device.is_connected()) {
        m_device.stop();
    }

    hc::util::logger::log("stopping PWM...");
    PWM::stop();

    return true;
}

void rgb_lights::shutdown() {
    if (!m_running) {
        hc::util::logger::err("early shutdown warning: please wait until init is finished");
        return;
    }

    m_device.stop();
}

bool rgb_lights::set_color_and_state(uint8_t r, uint8_t g, uint8_t b, bool limit) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    PWM::analog_write(PWM::PWM_PIN_R, r / 255.0f);
    PWM::analog_write(PWM::PWM_PIN_G, g / 255.0f);
    PWM::analog_write(PWM::PWM_PIN_B, b / 255.0f);
    
    static const int SEND_THRESHOLD = 17;

    if (limit) {
        if (m_num_sends_limited < SEND_THRESHOLD) {
            m_num_sends_limited++;
            return true;
        }

        m_num_sends_limited = 0;
    }
    
    m_state.set_r(r);
    m_state.set_g(g);
    m_state.set_b(b);

    hc::api::state new_state = m_device.get_state();
    new_state.set_data(m_state.serialize());

    try {
        m_device.set_state(new_state);
    } catch(hc::exception& e) {
        if (m_running) {
            hc::util::logger::err("failed to update device state: " + std::string(e.what()));
        }
    }

    return true;
}

void rgb_lights::turn_on_callback() {
    hc::util::logger::log("switching on!");

    if (m_program_running) {
        m_program_ptr->unpause();
    } else {
        //set_color_and_state(m_last_r, m_last_g, m_last_b);
        perform_fade(m_last_r, m_last_g, m_last_b);
    }
}

void rgb_lights::turn_off_callback() {
    hc::util::logger::log("switching off!");

    if (m_program_running) {
        m_program_ptr->pause();
    } else {
        m_last_r = m_state.get_r();
        m_last_g = m_state.get_g();
        m_last_b = m_state.get_b();
    }

    //set_color_and_state(0x00, 0x00, 0x00);
    perform_fade(0x00, 0x00, 0x00);
}

std::string rgb_lights::data_callback(std::string data) {
    hc::api::rgb_lights_state new_state;
    if (!new_state.parse(data)) {
        hc::util::logger::dbg("packet did not contain a valid RGBLights state!");
        return { 0x01 };
    }

    uint8_t res;

    if (new_state.get_program() != hc::api::rgb_lights_state::program::NONE) {
        if (m_state.get_program() == new_state.get_program()) {
            res = interrupt_program(new_state.get_data()) ? 0x00 : 0x02; // 0x02: failed to interrupt program
        } else {
            res = start_program(new_state.get_program()) ? 0x00 : 0x03; // 0x03: failed to start program
        }
    } else {
        if (m_state.get_program() != hc::api::rgb_lights_state::program::NONE) {
            stop_program();
        }

        res = set_color(new_state.get_r(), new_state.get_g(), new_state.get_b()) ? 0x00: 0x04; // 0x04: failed to set color
    }

    hc::api::state device_state = m_device.get_state();
    device_state.set_data(m_state.serialize());

    if (m_needs_power_on) {
        m_needs_power_on = false;
        device_state.set_power(hc::api::state::power::ON);
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_device.set_state(device_state);

    return { static_cast<char>(res) };
}

void rgb_lights::perform_fade(uint8_t target_r, uint8_t target_g, uint8_t target_b) {
    bool finished = false;

    while(!finished && m_running) {
        int r = m_state.get_r();
        int g = m_state.get_g();
        int b = m_state.get_b();

        static const int FADE_AMOUNT = 10;

        if (r < target_r) {
            r += FADE_AMOUNT;
            if (r > target_r) r = target_r; 
        }
        if (r > target_r) {
            r -= FADE_AMOUNT;
            if (r < target_r) r = target_r;
        }

        if (g < target_g) {
            g += FADE_AMOUNT;
            if (g > target_g) g = target_g; 
        }
        if (g > target_g) {
            g -= FADE_AMOUNT;
            if (g < target_g) g = target_g;
        }

        if (b < target_b) {
            b += FADE_AMOUNT;
            if (b > target_b) b = target_b; 
        }
        if (b > target_b) {
            b -= FADE_AMOUNT;
            if (b < target_b) b = target_b;
        }

        if (target_r == r && target_g == g && target_b == b) {
            finished = true;
        }

        set_color_and_state(r, g, b);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

bool rgb_lights::start_program(hc::api::rgb_lights_state::program program_type) {
    static const std::unordered_map<hc::api::rgb_lights_state::program, std::function<program*()>> programs = {
        { hc::api::rgb_lights_state::program::RAINBOW_FADE, []() { return new rainbow_fade_program(); }}
    };
    
    auto mit = programs.find(program_type);
    if (mit == programs.end()) {
        return false;
    }

    m_last_r = m_state.get_r();
    m_last_g = m_state.get_g();
    m_last_b = m_state.get_b();

    std::unique_ptr<program> program_ptr;
    program_ptr.reset(mit->second());

    if (m_program_running) {
        stop_program();
    }

    m_program_ptr = std::move(program_ptr);
    m_program_ptr->set_app(this);
    m_program_ptr->set_speed(m_state.get_speed());

    PWM::reset();

    m_state.set_program(program_type);

    m_program_thread = std::thread(&program::run, m_program_ptr.get());
    m_program_running = true;

    if (m_device.get_state().get_power() == hc::api::state::power::OFF) {
        m_needs_power_on = true;
    }

    hc::util::logger::log("program started!");

    return false;
}

void rgb_lights::stop_program() {
    if (m_program_running) {
        m_program_ptr->stop();
        m_program_thread.join();
        m_program_running = false;

        m_program_ptr.reset();

        m_state.set_program(hc::api::rgb_lights_state::program::NONE);

        //set_color(m_last_r, m_last_g, m_last_b);

        hc::util::logger::log("program stopped!");
    }
}

bool rgb_lights::interrupt_program(const std::string& data) {
    if (!m_program_running) {
        return false;
    }

    m_program_ptr->interrupt(data);

    return true;
}

bool rgb_lights::set_color(uint8_t r, uint8_t g, uint8_t b) {
    if (m_device.get_state().get_power() == hc::api::state::power::OFF) {
        m_needs_power_on = true;
    }

    PWM::analog_write(PWM::PWM_PIN_R, r / 255.0f);
    PWM::analog_write(PWM::PWM_PIN_G, g / 255.0f);
    PWM::analog_write(PWM::PWM_PIN_B, b / 255.0f);
    
    m_state.set_r(r);
    m_state.set_g(g);
    m_state.set_b(b);

    hc::util::logger::log("setting color! r: " + std::to_string(r) + " g: " + std::to_string(g) + " b: " + std::to_string(b));
    
    return true;
}