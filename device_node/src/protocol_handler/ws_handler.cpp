#include "ws_handler.h"

#include "device_handler.h"

#include <homecontroller/util/logger.h>
#include <homecontroller/exception.h>
#include <homecontroller/util/base64.h>
#include <homecontroller/api/info.h>

void ws_handler::init(const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& upgrade_request) {
    m_ws_wrapper_ptr = std::make_shared<hc::net::ws::server_wrapper>();
    m_ws_wrapper_ptr->wrap(conn_ptr);
    m_ws_wrapper_ptr->process_client_handshake(upgrade_request);
}

void ws_handler::on_destroyed(const state& state) {
    try {
        if (m_authenticated) {
            state.m_device_manager->remove_user(m_user_ptr->get_id(), shared_from_this());
        }
    } catch(hc::exception& e) {
        hc::util::logger::err("failed to disconnect user: " + std::string(e.what()));
    }
}

void ws_handler::send_notification(const std::string& device_id, const std::string& data) {
    hc::api::client_packet packet;
    packet.set_message_id(0x00000000);
    packet.set_device_id(device_id);
    packet.set_opcode(hc::api::client_packet::opcode::NOTIFICATION);
    packet.set_data_length(data.size());
    packet.set_data(data);

    m_ws_wrapper_ptr->send(packet.serialize());
}

void ws_handler::on_data(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data) {
    hc::util::logger::dbg("received user message over websocket");

    m_ws_wrapper_ptr->recv_external(data);
    if (m_ws_wrapper_ptr->is_closed()) {
        state.m_server->close_connection(conn_ptr);
        return;
    }

    hc::api::client_packet packet;

    try {
        packet.parse(m_ws_wrapper_ptr->get_last_message());
    } catch(hc::exception& e) {
        hc::util::logger::dbg("failed to parse packet: " + std::string(e.what()));
        m_ws_wrapper_ptr->close();
        state.m_server->close_connection(conn_ptr);
        return;
    }

    hc::api::client_packet res;
    bool need_send = true;

    if (packet.get_magic() != hc::api::info::MAGIC) {
        hc::util::logger::dbg("packet had invalid magic value");
        res = hc::api::client_packet(hc::api::client_packet::opcode::ERROR, { 0x01 }); // error code 0x01 for invalid protocol version
    }

    else if (!m_authenticated) {
        if (!authenticate(state, packet)) {
            res = hc::api::client_packet(hc::api::client_packet::opcode::ERROR, { 0x02 }); // error code 0x02 for auth fail
        }

        else {
            hc::util::logger::dbg("successfully authenticated!");
            m_authenticated = true;
            
            res = hc::api::client_packet(hc::api::client_packet::opcode::AUTHENTICATE, { 0x00 });
        }
    }

    else {
        try {
            std::shared_ptr<device> device_ptr = m_user_ptr->get_device(packet.get_device_id());
            
            if (device_ptr == nullptr) {
                res = hc::api::client_packet(hc::api::client_packet::opcode::ERROR, { 0x04 }); // error code 0x04 for device not connected
            }

            else {
                device_ptr->get_device_handler()->send_and_forward_response(m_ws_wrapper_ptr, m_ws_wrapper_ptr->get_last_message());
                need_send = false;
            }
        } catch(hc::exception& e) {
            hc::util::logger::dbg("failed to forward message to device: " + std::string(e.what()));
        }
    }

    if (need_send) {
        res.set_message_id(packet.get_message_id());
        res.set_device_id(packet.get_device_id());
        m_ws_wrapper_ptr->send(res.serialize());
    }
}

bool ws_handler::authenticate(const state& state, const hc::api::client_packet& packet) {
    if (packet.get_opcode() != hc::api::client_packet::opcode::AUTHENTICATE) {
        hc::util::logger::dbg("client never sent auth ticket");
        return false;
    }

    if (packet.get_data_length() < hc::api::info::TICKET_LENGTH) {
        hc::util::logger::dbg("bad ticket");
        return false;
    }

    try {
        hc::api::validate_user_response res = state.m_api_request_maker->validate_user(packet.get_data(), state.m_secret);
        m_user_ptr = state.m_device_manager->get_user(res.get_user_id());
    } catch(hc::exception& e) {
        hc::util::logger::dbg("failed to validate user ticket: " + std::string(e.what()));
        return false;
    }

    m_user_ptr->add_associated_handler(shared_from_this());

    for (auto& x : m_user_ptr->get_devices()) {
        send_notification(x.first, x.second->get_state().serialize());
    }

    return true;
}