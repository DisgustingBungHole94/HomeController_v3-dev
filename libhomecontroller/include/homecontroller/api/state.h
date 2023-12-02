#pragma once

#include <string>

namespace hc {
namespace api {
    
    class state {
        public:
            /*enum class type : uint8_t {
                DATA        = 0x00,
                CONNECT     = 0x01,
                DISCONNECT  = 0x02
            };*/

            enum class power : uint8_t {
                ON          = 0x00, 
                OFF         = 0x01
            };

            static std::size_t MIN_STATE_SIZE;

            state() 
                : /*m_type(type::DATA),*/ m_power(power::OFF), m_data("")
            {}

            ~state() {}

            void parse(const std::string& data);
            std::string serialize() const;

            /*void set_type(type type) { m_type = type; }
            type get_type() const { return m_type; }*/

            void set_power(power power) { m_power = power; }
            power get_power() const { return m_power; }

            void set_data(const std::string& data) { m_data = data; }
            const std::string& get_data() const { return m_data; }

        private:
            //type m_type;

            power m_power;
            std::string m_data;
    };

}
}