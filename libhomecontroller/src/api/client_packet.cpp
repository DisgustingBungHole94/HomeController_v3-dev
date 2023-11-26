#include "homecontroller/api/client_packet.h"

#include <iostream>

#include "homecontroller/exception.h"
#include "homecontroller/util/bits.h"
#include "homecontroller/api/info.h"
#include "homecontroller/util/logger.h"

namespace hc {
namespace api {

    uint8_t client_packet::_MIN_OPCODE = 0x00;
    uint8_t client_packet::_MAX_OPCODE = 0x07;
    std::size_t client_packet::_PACKET_HEADER_SIZE = 11 + info::DEVICE_ID_LENGTH;
    std::size_t client_packet::_MAX_DATA_LENGTH = 4096;

    void client_packet::parse(const std::string& data) {
        if (data.size() < _PACKET_HEADER_SIZE) {
            throw exception("packet too small", "hc::api::client_packet::parse");
        }

        std::size_t index = 0;

        // get magic
        m_magic = util::bits::bytes_to_u16(reinterpret_cast<const uint8_t*>(&data[index]));
        index += 2;

        // get message id
        m_messsage_id = util::bits::bytes_to_u32(reinterpret_cast<const uint8_t*>(&data[index]));
        index += 4;    

        // get opcode
        uint8_t opcode = static_cast<uint8_t>(data[index]);
        index += 1;

        if (opcode < _MIN_OPCODE || opcode > _MAX_OPCODE) {
            throw exception("invalid opcode", "hc::api::client_packet::parse");
        }

        m_opcode = static_cast<client_packet::opcode>(opcode);

        // get device id
        m_device_id = data.substr(index, info::DEVICE_ID_LENGTH);
        index += info::DEVICE_ID_LENGTH;

        // get data length
        m_data_length = util::bits::bytes_to_u32(reinterpret_cast<const uint8_t*>(&data[index])); //  39-43
        index += 4;

        if ((data.size() - _PACKET_HEADER_SIZE) != m_data_length) {
            throw exception("packet size mismatch", "hc::api::client_packet::parse");
        }

        if (m_data_length > _MAX_DATA_LENGTH) {
            throw exception("packet is too big", "hc::api::client_packet::parse");
        }

        m_data = data.substr(index);
    }

    std::string client_packet::serialize() {
        std::string data;
        data.reserve(_PACKET_HEADER_SIZE + m_data.size());

        // set magic
        data += hc::util::bits::u16_to_str(m_magic);

        // set message id
        data += hc::util::bits::u32_to_str(m_messsage_id);

        // set opcode
        data += hc::util::bits::u8_to_str(static_cast<uint8_t>(m_opcode));

        // set device id
        if (m_device_id.size() != info::DEVICE_ID_LENGTH) {
            throw exception("bad device id", "hc::api::client_packet::serialize");
        }

        data += m_device_id;

        // set data length
        if (m_data.size() > _MAX_DATA_LENGTH) {
            throw exception("packet data is too big", "hc::api::client_packet::serialize");
        }

        data += hc::util::bits::u32_to_str(static_cast<uint32_t>(m_data.size()));

        // set data
        data += m_data;

        return data;
    }

}
}