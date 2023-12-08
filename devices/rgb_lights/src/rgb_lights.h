#pragma once

#include <homecontroller/api/device.h>
#include <homecontroller/api/device_states/rgb_lights_state.h>

#include <thread>
#include <mutex>

#include "programs/program.h"

class rgb_lights {
    public:
        rgb_lights() 
            : m_running(false), m_program_running(false), m_needs_power_on(false)
        {}

        ~rgb_lights() {}

        bool start();
        void shutdown();

        bool set_color_and_state(uint8_t r, uint8_t g, uint8_t b);

    private:
        void turn_on_callback();
        void turn_off_callback();
        std::string data_callback(std::string data);

        void perform_fade(uint8_t target_r, uint8_t target_g, uint8_t target_b);

        bool start_program(hc::api::rgb_lights_state::program program_type);
        void stop_program();
        bool interrupt_program(const std::string& data);

        bool set_color(uint8_t r, uint8_t g, uint8_t b);

        hc::api::device m_device;
        bool m_running;

        hc::api::rgb_lights_state m_state;

        std::thread m_program_thread;
        std::unique_ptr<program> m_program_ptr;
        bool m_program_running;

        bool m_needs_power_on;

        uint8_t m_last_r;
        uint8_t m_last_g;
        uint8_t m_last_b;
        
        std::mutex m_mutex;
};