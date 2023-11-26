#include "homecontroller/api/device.h"

#include "homecontroller/util/logger.h"
#include "homecontroller/api/client_packet.h"
#include "homecontroller/api/info.h"

#include <iostream>

namespace hc {
namespace api {
    
    void device::run(bool is_on) {
        client_packet auth_req_packet;
        auth_req_packet.set_message_id(0xABCD1234);
        auth_req_packet.set_opcode(client_packet::opcode::AUTHENTICATE);
        auth_req_packet.set_device_id(m_device_id);

        std::string data;
        data.reserve(info::TICKET_LENGTH + 1);
        data += (is_on) ? 0x00 : 0x01;
        data += m_ticket;

        auth_req_packet.set_data_length(data.size());
        auth_req_packet.set_data(data);

        m_conn_ptr->send(auth_req_packet.serialize());

        std::string auth_res_data = m_conn_ptr->recv();
        if (m_conn_ptr->is_closed()) {
            hc::util::logger::log("disconnected from server");
            return;
        }

        client_packet auth_res_packet;
        auth_res_packet.parse(auth_res_data);

        if (auth_res_packet.get_opcode() != hc::api::client_packet::opcode::AUTHENTICATE) {
            hc::util::logger::err("node did not authorize client");

            m_conn_ptr->close();
            return;
        }

        m_running = true;

        while(m_running) {
            std::string req_data = m_conn_ptr->recv();
            if (m_conn_ptr->is_closed()) {
                hc::util::logger::log("disconnected from server");
                return;
            }
            client_packet req_packet;
            req_packet.parse(req_data);

            if (req_packet.get_device_id() != m_device_id) {
                hc::util::logger::err("packet device id mismatch!");

                m_conn_ptr->close();
                return;
            }

            switch(req_packet.get_opcode()) {
                case hc::api::client_packet::opcode::ON:
                    hc::util::logger::log("received turn on packet");
                    break;
                case hc::api::client_packet::opcode::OFF:
                    hc::util::logger::log("received turn off packet");
                    break;
                case hc::api::client_packet::opcode::DATA:
                    hc::util::logger::log("received data packet");
                    break;
                case hc::api::client_packet::opcode::GET_STATE:
                    hc::util::logger::log("received get state packet");
                    break;
                default:
                    break;
            }

            std::string res_data = "Hello, world!";

            client_packet res_packet;
            res_packet.set_message_id(req_packet.get_message_id());
            res_packet.set_opcode(hc::api::client_packet::opcode::RESPONSE);
            res_packet.set_device_id(m_device_id);
            res_packet.set_data_length(res_data.size());
            res_packet.set_data(res_data);

            m_conn_ptr->send(res_packet.serialize());
        }
    }

    void device::stop() {
        if (m_running) {
            m_conn_ptr->close();
            m_running = false;
        }
    }

}
}