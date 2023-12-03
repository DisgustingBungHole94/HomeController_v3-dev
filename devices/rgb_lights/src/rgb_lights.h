#pragma once

#include <homecontroller/api/device.h>
#include <homecontroller/api/device_states/rgb_lights_state.h>

#include <thread>

class rgb_lights {
    public:
        rgb_lights() 
            : m_running(false), m_power(false), m_program_running(false)
        {}

        ~rgb_lights() {}

        bool start();
        void shutdown();

    private:
        void turn_on_callback();
        void turn_off_callback();
        std::string data_callback(std::string data);

        void update_color();

        bool start_program(uint8_t id);
        bool stop_program();
        bool interrupt_program(const std::string& data);

        hc::api::device m_device;
        bool m_running;

        hc::api::rgb_lights_state m_state;
        bool m_power;

        std::thread m_program_thread;
        bool m_program_running;
};