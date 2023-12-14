#include "homecontroller/api/device.h"

#include "homecontroller/util/logger.h"
#include "homecontroller/api/client_packet.h"
#include "homecontroller/api/info.h"
#include "homecontroller/exception.h"

#include <iostream>

namespace hc {
namespace api {
    
    void device::run(state::power power, const std::string& state_data) {
        //state initial_state;
        m_state.set_power(power);
        m_state.set_data(state_data);

        std::string init_state_data = m_state.serialize();

        client_packet auth_req_packet;
        auth_req_packet.set_message_id(0xABCD1234);
        auth_req_packet.set_opcode(client_packet::opcode::AUTHENTICATE);
        auth_req_packet.set_device_id(m_device_id);

        std::string data;
        data.reserve(info::TICKET_LENGTH + init_state_data.length());
        //data += (is_on) ? 0x00 : 0x01;
        data += m_ticket;
        data += init_state_data;

        auth_req_packet.set_data_length(data.size());
        auth_req_packet.set_data(data);

        util::logger::dbg("sending auth packet...");

        m_conn_ptr->send(auth_req_packet.serialize());

        std::string auth_res_data = m_conn_ptr->recv();
        if (m_conn_ptr->is_closed()) {
            util::logger::log("disconnected from server");
            return;
        }

        client_packet auth_res_packet;
        auth_res_packet.parse(auth_res_data);

        if (auth_res_packet.get_opcode() != hc::api::client_packet::opcode::AUTHENTICATE) {
            util::logger::err("node did not authorize client");

            m_conn_ptr->close();
            return;
        }

        util::logger::dbg("successfully authenticated!");

        m_running = true;

        util::logger::log("device started, listening for requests");

        while(m_running) {
            std::string req_data = m_conn_ptr->recv();
            if (m_conn_ptr->is_closed()) {
                util::logger::log("disconnected from server");
                return;
            }

            // device is pinged regularly to check connection
            if (req_data.size() == 1 && req_data[0] == 0x00) {
                m_conn_ptr->send({ 0x00 });
                continue;
            }

            client_packet req_packet;

            try {
                req_packet.parse(req_data);
            } catch(hc::exception& e) {
                util::logger::err("failed to parse packet: " + std::string(e.what()));
                continue;
            }

            if (req_packet.get_device_id() != m_device_id) {
                util::logger::err("packet device id mismatch!");

                m_conn_ptr->close();
                return;
            }

            std::string res_data = { 0x00 };

            switch(req_packet.get_opcode()) {
                case hc::api::client_packet::opcode::ON:
                    hc::util::logger::dbg("received ON packet");
                    m_turn_on_callback();
                    m_state.set_power(hc::api::state::power::ON);
                    send_notify_packet();
                    break;
                case hc::api::client_packet::opcode::OFF:
                    hc::util::logger::dbg("received OFF packet");
                    m_turn_off_callback();
                    m_state.set_power(hc::api::state::power::OFF);
                    send_notify_packet();
                    break;
                case hc::api::client_packet::opcode::DATA:
                    hc::util::logger::dbg("received DATA packet");
                    res_data = m_data_callback(req_packet.get_data());
                    break;
                default:
                    break;
            }

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

    void device::set_state(const state& state) {
        m_state = state;
        send_notify_packet();
    }

    void device::send_notify_packet() {
        std::string state_data = m_state.serialize();
       
       client_packet notify_packet;
       notify_packet.set_message_id(0x00000000);
       notify_packet.set_opcode(hc::api::client_packet::opcode::NOTIFICATION);
       notify_packet.set_device_id(m_device_id);
       notify_packet.set_data_length(state_data.length());
       notify_packet.set_data(state_data);

       m_conn_ptr->send(notify_packet.serialize());
    }

}
}