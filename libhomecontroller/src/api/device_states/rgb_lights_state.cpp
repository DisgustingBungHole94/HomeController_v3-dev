#include "homecontroller/api/device_states/rgb_lights_state.h"

#include "homecontroller/util/bits.h"
#include "homecontroller/util/logger.h"

#include <cstring>

namespace hc {
namespace api {

    uint8_t rgb_lights_state::_MIN_PROGRAM = 0x00;
    uint8_t rgb_lights_state::_MAX_PROGRAM = 0x04;

    std::size_t rgb_lights_state::_MIN_STATE_SIZE = 8;

    bool rgb_lights_state::parse(const std::string& data) {
        if (data.size() < _MIN_STATE_SIZE) {
            return false;
        }

        std::size_t index = 0;

        // get r
        m_r = static_cast<uint8_t>(data[index]);
        index += 1;

        // get g
        m_g = static_cast<uint8_t>(data[index]);
        index += 1;

        // get b
        m_b = static_cast<uint8_t>(data[index]);
        index += 1;

        // get speed
        uint32_t speed_bytes = util::bits::bytes_to_u32(reinterpret_cast<const uint8_t*>(&data[index]));
        index += 4;

        union {
            uint32_t from;
            float to;
        } pun = { .from = speed_bytes };
       
        m_speed = pun.to;

        uint8_t program = static_cast<uint8_t>(data[index]);

        if (program < _MIN_PROGRAM || program > _MAX_PROGRAM) {
            return false;
        }

        m_program = static_cast<rgb_lights_state::program>(program);
        index += 1;

        m_data = data.substr(index);

        return true;
    }

    std::string rgb_lights_state::serialize() const {
        std::string data;
        data.reserve(_MIN_STATE_SIZE + m_data.size());

        data += util::bits::u8_to_str(m_r);
        data += util::bits::u8_to_str(m_g);
        data += util::bits::u8_to_str(m_b);

        union {
            float from;
            uint32_t to;
        } pun = { .from = m_speed };
       
        data += util::bits::u32_to_str(pun.to);

        data += util::bits::u8_to_str(static_cast<uint8_t>(m_program));

        data += m_data;

        return data;
    }

}
}