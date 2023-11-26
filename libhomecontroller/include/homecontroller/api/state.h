#pragma once

#include <string>

namespace hc {
namespace api {
    
    class state {
        public:
            enum class type : uint8_t {
                DATA        = 0x00, 
                DISCONNECT  = 0x01
            };

            enum class power : uint8_t {
                ON          = 0x00, 
                OFF         = 0x01
            };

            state() 
                : m_type(hc::api::state::type::DATA), m_power(hc::api::state::power::OFF), m_data("")
            {}

            ~state() {}

            void parse(const std::string& data);
            std::string serialize() const;

            void set_type(type type) { m_type = type; }
            type get_type() const { return m_type; }

            void set_power(power power) { m_power = power; }
            power get_power() const { return m_power; }

            void set_data(const std::string& data) { m_data = data; }
            const std::string& get_data() const { return m_data; }

        private:
            type m_type;

            power m_power;
            std::string m_data;
    };

}
}