#pragma once

#include <string>
#include <vector>

namespace hc {
namespace api {

    class client_packet {
        public:
            enum class opcode : uint8_t {
                AUTHENTICATE    = 0x00,
                ON              = 0x01,
                OFF             = 0x02,
                DATA            = 0x03,
                GET_STATE       = 0x04,
                RESPONSE        = 0x05,
                ERROR           = 0x06,
                NOTIFICATION    = 0x07
            };

            client_packet() 
                : client_packet(opcode::ERROR, "")
            {}

            client_packet(opcode opcode, const std::string& data) 
                : client_packet(0, opcode, "", data)
            {}

            client_packet(uint32_t message_id, opcode opcode, const std::string& device_id, const std::string& data)
                : m_magic(0xBEEF), m_messsage_id(message_id), m_opcode(opcode), m_device_id(device_id), m_data_length(data.size()), m_data(data)
            {}

            ~client_packet() {}

            void parse(const std::string& data);
            std::string serialize();

            void set_magic(uint16_t magic) { m_magic = magic; }
            uint16_t get_magic() const { return m_magic; }

            void set_message_id(uint32_t message_id) { m_messsage_id = message_id; }
            uint32_t get_message_id() const { return m_messsage_id; }

            void set_opcode(opcode opcode) { m_opcode = opcode; }
            opcode get_opcode() const { return m_opcode; }

            void set_device_id(const std::string& device_id) { m_device_id = device_id; }
            const std::string& get_device_id() const { return m_device_id; }

            void set_data_length(uint32_t data_length) { m_data_length = data_length; }
            uint32_t get_data_length() const { return m_data_length; }

            void set_data(const std::string& data) { m_data = data; }
            const std::string& get_data() const { return m_data; }

        private:
            uint16_t m_magic;  // 2 bytes
            uint32_t m_messsage_id;     // 4 bytes
            opcode m_opcode;            // 1 byte
            std::string m_device_id;    // 32 bytes (defined in hc::api::info::DEVICE_ID_LENGTH)
            uint32_t m_data_length;     // 4 bytes

            static uint8_t _MIN_OPCODE;
            static uint8_t _MAX_OPCODE;

            static std::size_t _PACKET_HEADER_SIZE;

            std::string m_data;
            static std::size_t _MAX_DATA_LENGTH;
    };

}
}