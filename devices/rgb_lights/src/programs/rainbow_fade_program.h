#pragma once

#include "program.h"

class rgb_lights;

class rainbow_fade_program : public program {
    public:
        rainbow_fade_program()
            : m_color_status(color_status::NONE), m_r(0), m_g(0), m_b(0)
        {}

        ~rainbow_fade_program() {}

        void loop() override;
        void on_interrupt() override;
        void on_stop() override;

    private:
        int m_r;
        int m_b;
        int m_g;

        bool set_color(int r, int g, int b);

        enum class color_status {
            NONE,
            RED,
            ORANGE,
            YELLOW,
            GREEN,
            CYAN,
            BLUE,
            PURPLE
        } m_color_status;
};