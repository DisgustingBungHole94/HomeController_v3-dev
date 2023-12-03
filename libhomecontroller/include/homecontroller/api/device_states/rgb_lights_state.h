#pragma once

#include "homecontroller/api/device_states/device_state.h"

namespace hc {
namespace api {
    class rgb_lights_state : public device_state {
        public:
            enum class program : uint8_t {
                NONE =              0x00,
                RAINBOW_FADE =      0x01,
                PSYCHEDELIC_FADE =  0x02,
                GUITAR_SYNC =       0x03,
                MUSIC_SYNC =        0x04
            };

            rgb_lights_state() 
                : rgb_lights_state(0x00, 0x00, 0x00, 0.0f, program::NONE)
            {}

            rgb_lights_state(uint8_t r, uint8_t g, uint8_t b, float speed, program program)
                : m_r(r), m_g(g), m_b(b), m_speed(speed), m_program(program)
            {}

            ~rgb_lights_state() {}

            bool parse(const std::string& data) override;
            std::string serialize() const override;

            void set_r(uint8_t r) { m_r = r; }
            uint8_t get_r() { return m_r; }

            void set_g(uint8_t g) { m_g = g; }
            uint8_t get_g() { return m_g; }

            void set_b(uint8_t b) { m_b = b; }
            uint8_t get_b() { return m_b; }

            void set_speed(float speed) { m_speed = speed; }
            float get_speed() { return m_speed; }

            void set_program(program program) { m_program = program; }
            program get_program() { return m_program; }

            void set_data(const std::string& data) { m_data = data; }
            const std::string& get_data() { return m_data; }

        private:
            uint8_t m_r;
            uint8_t m_g;
            uint8_t m_b;

            float m_speed;
            
            program m_program;

            std::string m_data;

            static uint8_t _MIN_PROGRAM;
            static uint8_t _MAX_PROGRAM;

            static std::size_t _MIN_STATE_SIZE;
    };

}
}