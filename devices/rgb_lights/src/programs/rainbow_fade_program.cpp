#include "rainbow_fade_program.h"

#include "../rgb_lights.h"

#include <iostream>

void rainbow_fade_program::loop() {
    switch(m_color_status) {
        case color_status::RED:
            if (set_color(255, 0, 0)) m_color_status = color_status::ORANGE;
            break;
        case color_status::ORANGE:
            if (set_color(255, 69, 0)) m_color_status = color_status::YELLOW;
            break;
        case color_status::YELLOW:
            if (set_color(255, 255, 0)) m_color_status = color_status::GREEN;
            break;
        case color_status::GREEN:
            if (set_color(0, 255, 0)) m_color_status = color_status::CYAN;
            break;
        case color_status::CYAN:
            if (set_color(0, 255, 255)) m_color_status = color_status::BLUE;
            break;
        case color_status::BLUE:
            if (set_color(0, 0, 255)) m_color_status = color_status::PURPLE;
            break;
        case color_status::PURPLE:
            if (set_color(255, 0, 255)) m_color_status = color_status::RED;
            break;
        default:
            m_color_status = color_status::RED;
            break;
    }

    m_app->set_color_and_state(m_r, m_g, m_b, true);
    

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

void rainbow_fade_program::on_interrupt() {}

void rainbow_fade_program::on_stop() {}

bool rainbow_fade_program::set_color(int r, int g, int b) {
    if (r == m_r && g == m_g && b == m_b) {
        return true;
    }

    if (r > m_r) m_r += 1;
    if (r < m_r) m_r -= 1;

    if (g > m_g) m_g += 1;
    if (g < m_g) m_g -= 1;

    if (b > m_b) m_b += 1;
    if (b < m_b) m_b -= 1;

    return false;
}