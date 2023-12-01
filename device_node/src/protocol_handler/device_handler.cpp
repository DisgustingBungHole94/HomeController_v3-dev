#include "device_handler.h"

#include "ws_handler.h"

#include <homecontroller/http/response.h>
#include <homecontroller/api/info.h>
#include <homecontroller/util/logger.h>
#include <homecontroller/exception.h>
#include <chrono>

void device_handler::init(const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& upgrade_request) {
    m_conn_ptr = conn_ptr;

    hc::http::response upgrade_response;
    upgrade_response.set_status("101 Switching Protocols");
    upgrade_response.add_header("Connection", "upgrade");
    upgrade_response.add_header("Upgrade", hc::api::info::DEVICE_UPGRADE_SCHEMA + "/" + hc::api::info::VERSION);

    conn_ptr->send(upgrade_response.str());
}

void device_handler::on_destroyed(const state& state) {
    try {
        if (m_authenticated) {
            m_user_ptr->remove_device(m_device_ptr->get_id());

            hc::api::state disconnect_state;
            disconnect_state.set_type(hc::api::state::type::DISCONNECT);
            std::string data = disconnect_state.serialize();

            for(auto& x : m_user_ptr->get_associated_handlers()) {
                try {
                    x->send_notification(m_device_ptr->get_id(), data);
                } catch(hc::exception& e) {
                    hc::util::logger::err("failed to send disconnect notification: " + std::string(e.what()));
                }
            }
        }
    } catch(hc::exception& e) {
        hc::util::logger::err("failed to disconnect device: " + std::string(e.what()));
    }
}

void device_handler::send_and_forward_response(std::weak_ptr<hc::net::ws::server_wrapper> conn_hdl, const std::string& data) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_user_message_queue.size() > 1000) {
        throw hc::exception("too many messages in queue", "device_handler::send_and_forward_response");
    }

    m_user_message_queue.push(conn_hdl);

    m_conn_ptr->send(data);
}

void device_handler::on_data(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data) {
    hc::util::logger::dbg("received device message");
    
    hc::api::client_packet packet;

    try {
        packet.parse(data);
    } catch(hc::exception& e) {
        hc::util::logger::dbg("failed to parse packet: " + std::string(e.what()));
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

    else if (packet.get_opcode() == hc::api::client_packet::opcode::NOTIFICATION) {
        hc::api::state state;

        try {
            state.parse(packet.get_data());
            m_device_ptr->set_state(state);

            for (auto& x : m_user_ptr->get_associated_handlers()) {
                x->send_notification(m_device_ptr->get_id(), packet.get_data());
            }
        } catch(hc::exception& e) {
            hc::util::logger::dbg("failed to forward device notification: " + std::string(e.what()));
        }

        need_send = false;
    }

    else if (packet.get_opcode() == hc::api::client_packet::opcode::RESPONSE) {
        if (m_user_message_queue.empty()) {
            state.m_server->close_connection(m_conn_ptr);
            throw hc::exception("unrecoverable error; device is misbehaving", "hc::device_handler::on_data");
        }

        std::weak_ptr<hc::net::ws::server_wrapper> user_conn_hdl = m_user_message_queue.front();
        std::shared_ptr<hc::net::ws::server_wrapper> user_conn_ptr;

        if (!(user_conn_ptr = user_conn_hdl.lock()) || user_conn_ptr->is_closed()) {
            hc::util::logger::dbg("failed to forward message, user disconnected");
        } else {
            try {
                user_conn_ptr->send(data);
            } catch(hc::exception& e) {
                hc::util::logger::dbg("failed to forward message to user: " + std::string(e.what()));
            }
        }

        m_user_message_queue.pop();
        need_send = false;
    } else {
        hc::util::logger::dbg("device sent packet with invalid opcode");
        res = hc::api::client_packet(hc::api::client_packet::opcode::ERROR, { 0x03 }); // error code 0x03 for bad opcode
    }

    if (need_send) {
        res.set_message_id(packet.get_message_id());
        res.set_device_id(packet.get_device_id());

        m_conn_ptr->send(res.serialize());
    }
}

bool device_handler::authenticate(const state& state, const hc::api::client_packet& packet) {
    if (packet.get_opcode() != hc::api::client_packet::opcode::AUTHENTICATE) {
        hc::util::logger::dbg("client never sent auth packet");
        return false;
    }
    
    if (packet.get_data_length() < hc::api::info::TICKET_LENGTH + hc::api::state::MIN_STATE_SIZE) {
        hc::util::logger::dbg("bad auth packet");
        return false;
    }

    if (packet.get_data().size() < hc::api::info::TICKET_LENGTH + hc::api::state::MIN_STATE_SIZE) {
        hc::util::logger::dbg("bad auth packet");
        return false;
    }

    std::string state_data = packet.get_data().substr(hc::api::info::TICKET_LENGTH);
    hc::api::state initial_state;

    try {
        initial_state.parse(state_data);
    } catch(hc::exception& e) {
        hc::util::logger::dbg("failed to parse initial device state: " + std::string(e.what()));
        return false;
    }

    std::string ticket = packet.get_data().substr(0, hc::api::info::TICKET_LENGTH);

    try {
        hc::api::validate_device_response res = state.m_api_request_maker->validate_device(ticket, state.m_secret);

        m_user_ptr = state.m_device_manager->get_user(res.get_user_id());
        m_device_ptr = m_user_ptr->add_device(res.get_device_id(), shared_from_this());
        m_device_ptr->set_state(initial_state);

        hc::util::logger::dbg("device is powered " + std::string((initial_state.get_power() == hc::api::state::power::ON) ? "on" : "off"));
        hc::util::logger::dbg("device state data: " + initial_state.get_data());
    } catch(hc::exception& e) {
        hc::util::logger::dbg("failed to validate device ticket: " + std::string(e.what()));
        return false;
    }

    for(auto& x : m_user_ptr->get_associated_handlers()) {
        x->send_notification(m_device_ptr->get_id(), packet.get_data());
    }

    return true;
}