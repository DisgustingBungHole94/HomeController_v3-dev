#include "homecontroller/api/state.h"

#include "homecontroller/exception.h"
#include "homecontroller/util/bits.h"

namespace hc {
namespace api {

    std::size_t state::MIN_STATE_SIZE = 1;

    void state::parse(const std::string& data) {
        if (data.size() < MIN_STATE_SIZE) {
            throw exception("state too small", "hc::api::state::parse");
        }

        /*uint8_t type = static_cast<uint8_t>(data[0]);
        if (type < 0 || type > 2) {
            throw exception("invalid state type", "hc::api::state::parse");
        }

        m_type = static_cast<state::type>(type);*/

        uint8_t power = static_cast<uint8_t>(data[0]);
        if (power < 0 || power > 1) {
            throw exception("invalid power code", "hc::api::state::parse");
        }

        m_power = static_cast<state::power>(power);

        m_data = data.substr(1);
    }

    std::string state::serialize() const {
        std::string data;
        data.reserve(MIN_STATE_SIZE + m_data.size());

        //data += hc::util::bits::u8_to_str(static_cast<uint8_t>(m_type));

        data += hc::util::bits::u8_to_str(static_cast<uint8_t>(m_power));

        data += m_data;

        return data;
    }

}
}

