#pragma once

#include <thread>

class light_strip {
    public:
        light_strip() {}
        ~light_strip() {}

        bool run();
        void shutdown();

    private:
        void update_color();

        struct info {
            uint8_t m_r = 0;
            uint8_t m_g = 0;
            uint8_t m_b = 0;

            float m_speed = 1.0f;

            bool m_program_running = false;

            std::thread m_program_thread;
        } m_info;
};