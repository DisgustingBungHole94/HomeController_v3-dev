#include "program.h"

#include <iostream>


void program::run() {
    m_state = program_state::RUNNING;

    on_start();

    while(m_state != program_state::STOPPED) {
        if (m_state == program_state::PAUSED) {
            continue;
        } else if (m_state == program_state::INTERRUPT) {
            on_interrupt();
            m_state = program_state::RUNNING;
        }

        loop();
    }
    
    on_stop();
}

void program::interrupt(const std::string& data) {
    m_interrupt_data = data;
    m_state = program_state::INTERRUPT;
}

void program::pause() {
    m_state = program_state::PAUSED;
}

void program::unpause() {
    m_state = program_state::RUNNING;
}

void program::stop() {
    m_state = program_state::STOPPED;
}

void program::on_start() {}
void program::loop() {}
void program::on_interrupt() {}
void program::on_stop() {}