#include "ws_handler.h"

#include "device_handler.h"

#include <homecontroller/util/logger.h>
#include <homecontroller/exception.h>
#include <homecontroller/util/base64.h>
#include <homecontroller/api/info.h>

void ws_handler::send_upgrade_response(const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& upgrade_request) {
    m_ws_wrapper.init();

    m_ws_wrapper.use(conn_ptr);
    m_ws_wrapper.process_client_handshake(conn_ptr, upgrade_request);
    m_ws_wrapper.unuse();

    //m_ws_wrapper_ptr->wrap(conn_ptr);
    //m_ws_wrapper_ptr->process_client_handshake(upgrade_request);
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

void ws_handler::send_response(const std::string& data) {
    hc::net::ssl::server_conn_ptr conn_ptr;
    if (!(conn_ptr = m_conn_hdl.lock())) {
        throw hc::exception("invalid connection handle", "ws_handler::send_response");
    }

    m_ws_wrapper.use(conn_ptr);
    m_ws_wrapper.send(data);
    m_ws_wrapper.unuse();
}

void ws_handler::send_connect_packet(const std::string& device_id, const std::string& data) {
    hc::net::ssl::server_conn_ptr conn_ptr;
    if (!(conn_ptr = m_conn_hdl.lock())) {
        throw hc::exception("invalid connection handle", "ws_handler::send_connect_packet");
    }

    send_connect_packet(conn_ptr, device_id, data);
}

void ws_handler::send_connect_packet(hc::net::ssl::server_conn_ptr conn_ptr, const std::string& device_id, const std::string& data) {
    hc::api::client_packet packet;
    packet.set_message_id(0x00000000);
    packet.set_device_id(device_id);
    packet.set_opcode(hc::api::client_packet::opcode::CONNECT);
    packet.set_data_length(data.size());
    packet.set_data(data);

    m_ws_wrapper.use(conn_ptr);
    m_ws_wrapper.send(packet.serialize());
    m_ws_wrapper.unuse();
}

void ws_handler::send_disconnect_packet(const std::string& device_id) {
    hc::net::ssl::server_conn_ptr conn_ptr;
    if (!(conn_ptr = m_conn_hdl.lock())) {
        throw hc::exception("invalid connection handle", "ws_handler::send_disconnect_packet");
    }
    
    hc::api::client_packet packet;
    packet.set_message_id(0x00000000);
    packet.set_device_id(device_id);
    packet.set_opcode(hc::api::client_packet::opcode::DISCONNECT);

    m_ws_wrapper.use(conn_ptr);
    m_ws_wrapper.send(packet.serialize());
    m_ws_wrapper.unuse();
}

void ws_handler::send_notification_packet(const std::string& device_id, const std::string& data) {
    hc::api::client_packet packet;
    packet.set_message_id(0x00000000);
    packet.set_device_id(device_id);
    packet.set_opcode(hc::api::client_packet::opcode::NOTIFICATION);
    packet.set_data_length(data.size());
    packet.set_data(data);

    m_ws_wrapper.send(packet.serialize());
}

void ws_handler::on_data(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr) {
    hc::util::logger::dbg("received user message");

    m_ws_wrapper.recv();
    if (m_ws_wrapper.is_closed()) {
        state.m_server->close_connection(conn_ptr);
        return;
    }

    for (std::size_t i = 0; i < m_ws_wrapper.get_message_log().size(); i++) {
        std::string data = m_ws_wrapper.get_message_log()[i];

        hc::api::client_packet packet;
        try {
            packet.parse(data);
        } catch(hc::exception& e) {
            hc::util::logger::dbg("failed to parse packet: " + std::string(e.what()));
            
            m_ws_wrapper.close();
            state.m_server->close_connection(conn_ptr);
            return;
        }

        hc::api::client_packet res;
        bool need_send = true;

        if (packet.get_magic() != hc::api::info::MAGIC) {
            hc::util::logger::dbg("packet had invalid magic value");
            res = hc::api::client_packet(hc::api::client_packet::opcode::ERROR, { 0x01 }); // error code 0x01 for invalid protocol version
        }

        else {

            switch(packet.get_opcode()) {
                case hc::api::client_packet::opcode::AUTHENTICATE:
                    res = handle_authenticate(state, packet);
                    break;
                default:
                    if (!send_to_device(packet.get_device_id(), data)) {
                        // if device not found
                        res = hc::api::client_packet(hc::api::client_packet::opcode::ERROR, { 0x04 });
                    } else {
                        need_send = false;
                    }
                    break;
            }

        }

        if (need_send) {
            res.set_message_id(packet.get_message_id());
            res.set_device_id(packet.get_device_id());

            m_ws_wrapper.send(res.serialize());
        }
    }
}

hc::api::client_packet ws_handler::handle_authenticate(const state& state, const hc::api::client_packet& packet) {
    hc::util::logger::dbg("authenticating user...");
    
    hc::api::client_packet err_packet(hc::api::client_packet::opcode::ERROR, { 0x02 });
    
    if (packet.get_opcode() != hc::api::client_packet::opcode::AUTHENTICATE) {
        hc::util::logger::dbg("client never sent auth ticket");
        return err_packet;
    }

    if (packet.get_data_length() < hc::api::info::TICKET_LENGTH) {
        hc::util::logger::dbg("bad ticket");
        return err_packet;
    }

    hc::api::validate_user_response res;
    try {
        res = state.m_api_request_maker->validate_user(packet.get_data(), state.m_secret);
    } catch(hc::exception& e) {
        hc::util::logger::dbg("failed to validate user ticket: " + std::string(e.what()));
        return err_packet;
    }

    m_user_ptr = state.m_device_manager->get_user(res.get_user_id());
    m_user_ptr->add_associated_handler(shared_from_this());

    for (auto& x : m_user_ptr->get_devices()) {
        send_connect_packet(x.first, x.second->get_state().serialize());
    }

    m_authenticated = true;

    hc::util::logger::dbg("user authenticated!");

    return hc::api::client_packet(hc::api::client_packet::opcode::AUTHENTICATE, { 0x00 });
}

bool ws_handler::send_to_device(const std::string& device_id, const std::string& data) {
    std::shared_ptr<device> device_ptr = m_user_ptr->get_device(device_id);
    if (device_ptr == nullptr) {
        return false;
    }

    try {
        device_ptr->get_device_handler()->send_and_forward_response(shared_from_this(), data);
    
        hc::util::logger::dbg("[" + device_ptr->get_id() + "] <- User");
    } catch(hc::exception& e) {
        hc::util::logger::dbg("failed to forward message to device: " + std::string(e.what()));
    }

    return true;
}