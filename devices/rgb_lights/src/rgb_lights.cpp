#include "rgb_lights.h"

#include "pwm.h"

#include <homecontroller/api/request_maker.h>
#include <homecontroller/exception.h>
#include <homecontroller/util/logger.h>

bool rgb_lights::start() {
    hc::util::logger::enable_debug();

    hc::util::logger::log("RGBLights v0.0.0 by Josh Dittmer");

    if (!PWM::init("pwm-test")) {
        hc::util::logger::csh("failed to start PWM!");
        return false;
    }

    PWM::reset_pins();

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

        update_color_thread = std::thread(std::bind(&rgb_lights::update_color, this));
        update_color_thread_started = true;

        hc::api::state::power power_state = (m_power) ? hc::api::state::power::ON : hc::api::state::power::OFF;
        m_device.run(power_state, m_state.serialize());
    } catch(hc::exception& e) {
        hc::util::logger::err("device exception occurred: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");
        m_device.stop();
    }

    m_running = false;

    hc::util::logger::log("shutting down...");

    if (api.is_connected()) {
        api.disconnect();
    }

    if (m_device.is_connected()) {
        m_device.stop();
    }

    if (update_color_thread_started) {
        hc::util::logger::log("stopping update color thread...");
        update_color_thread.join();
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

void rgb_lights::turn_on_callback() {
    hc::util::logger::log("switching on");
}

void rgb_lights::turn_off_callback() {
    hc::util::logger::log("switching off");
}

std::string rgb_lights::data_callback(std::string data) {
    hc::api::rgb_lights_state state;
    if (!state.parse(data)) {
        hc::util::logger::dbg("packet did not contain a valid RGBLights state!");
        return { 0x01 };
    }

    hc::util::logger::log("state R: " + std::to_string(state.get_r()));
    hc::util::logger::log("state G: " + std::to_string(state.get_g()));
    hc::util::logger::log("state B: " + std::to_string(state.get_b()));
    hc::util::logger::log("state speed: " + std::to_string(state.get_speed()));
    hc::util::logger::log("state program: " + std::to_string((uint8_t)state.get_program()));

    hc::api::state curr_state = m_device.get_state();
    curr_state.set_data(state.serialize());

    m_device.set_state(curr_state);

    return { 0x00 };
}

void rgb_lights::update_color() {
    while(m_running) {

    }
}

bool rgb_lights::start_program(uint8_t id) {
    return false;
}

bool rgb_lights::stop_program() {
    return false;
}

bool rgb_lights::interrupt_program(const std::string& data) {
    return false;
}