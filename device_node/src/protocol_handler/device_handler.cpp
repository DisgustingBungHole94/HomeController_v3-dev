#include "device_handler.h"

#include "ws_handler.h"

#include <homecontroller/http/response.h>
#include <homecontroller/api/info.h>
#include <homecontroller/util/logger.h>
#include <homecontroller/exception.h>
#include <chrono>

void device_handler::send_upgrade_response(const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& upgrade_request) {
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

            for(auto& x : m_user_ptr->get_associated_handlers()) {
                try {
                    x->send_disconnect_packet(m_device_ptr->get_id());
                } catch(hc::exception& e) {
                    hc::util::logger::err("failed to send disconnect notification: " + std::string(e.what()));
                }
            }
        }
    } catch(hc::exception& e) {
        hc::util::logger::err("failed to disconnect device: " + std::string(e.what()));
    }

    /*if (m_should_check_connection) {
        hc::util::logger::dbg("stopping check connection thread...");
        m_should_check_connection = false;
        m_check_connection_thread.join();
    }*/
}

void device_handler::send_and_forward_response(std::weak_ptr<ws_handler> user_handler_ptr, const std::string& data) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_user_queue.size() > 1000) {
        throw hc::exception("too many messages in queue", "device_handler::send_and_forward_response");
    }

    hc::net::ssl::server_conn_ptr conn_ptr;
    if (!(conn_ptr = m_conn_hdl.lock())) {
        throw hc::exception("device connection expired", "device_handler::send_and_forward_response");
    } 

    m_user_queue.push(user_handler_ptr);

    std::cout << "start device send" << std::endl;
    conn_ptr->send(data);
    std::cout << "end device send" << std::endl;
}

void device_handler::on_data(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr) {
    std::string data = conn_ptr->recv();
    if (conn_ptr->is_closed()) {
        hc::util::logger::dbg("device connection closed!");
        return;
    }

    // device pings regularly to prevent timeout
    if (data.size() == 1 && data[0] == 0x00) {
        return;
    }

    hc::util::logger::dbg("received device message");
    
    hc::api::client_packet packet;
    try {
        packet.parse(data);
    } catch(hc::exception& e) {
        hc::util::logger::dbg("failed to parse packet: " + std::string(e.what()));
        
        state.m_server->close_connection(conn_ptr);
        return;
    }

    handle_packet(state, conn_ptr, data, packet);
}

void device_handler::handle_packet(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data, const hc::api::client_packet& packet) {
    hc::api::client_packet res;
    bool need_send = true;
    
    if (packet.get_magic() != hc::api::info::MAGIC) {
        hc::util::logger::dbg("packet had invalid magic value");
        res = hc::api::client_packet(hc::api::client_packet::opcode::ERROR, { 0x01 }); // error code 0x01 for invalid protocol version
        return;
    }

    switch(packet.get_opcode()) {
        case hc::api::client_packet::opcode::AUTHENTICATE:
            res = handle_authenticate(state, packet);
            break;
        case hc::api::client_packet::opcode::NOTIFICATION:
            handle_notification(packet);
            need_send = false;
            break;
        case hc::api::client_packet::opcode::RESPONSE:
            handle_response(state, conn_ptr, data);
            need_send = false;
            break;
        default:
            hc::util::logger::dbg("device sent packet with invalid opcode");
            res = hc::api::client_packet(hc::api::client_packet::opcode::ERROR, { 0x03 });
            break;
    }


    if (need_send) {
        res.set_message_id(packet.get_message_id());
        res.set_device_id(packet.get_device_id());

        conn_ptr->send(res.serialize());
    }
}


hc::api::client_packet device_handler::handle_authenticate(const state& state, const hc::api::client_packet& packet) {
    hc::api::client_packet err_packet(hc::api::client_packet::opcode::ERROR, { 0x02 });
    
    if (m_authenticated) {
        return err_packet;
    }

    hc::util::logger::dbg("authenticating device...");

    if (packet.get_opcode() != hc::api::client_packet::opcode::AUTHENTICATE) {
        hc::util::logger::dbg("client never sent auth packet");
        return err_packet;
    }
    
    if (packet.get_data_length() < hc::api::info::TICKET_LENGTH + hc::api::state::MIN_STATE_SIZE) {
        hc::util::logger::dbg("bad auth packet");
        return err_packet;
    }

    if (packet.get_data().size() < hc::api::info::TICKET_LENGTH + hc::api::state::MIN_STATE_SIZE) {
        hc::util::logger::dbg("bad auth packet");
        return err_packet;
    }

    std::string state_data = packet.get_data().substr(hc::api::info::TICKET_LENGTH);
    
    hc::api::state initial_state;
    try {
        initial_state.parse(state_data);
    } catch(hc::exception& e) {
        hc::util::logger::dbg("failed to parse initial device state: " + std::string(e.what()));
        return err_packet;
    }

    std::string ticket = packet.get_data().substr(0, hc::api::info::TICKET_LENGTH);

    try {
        hc::api::validate_device_response res = state.m_api_request_maker->validate_device(ticket, state.m_secret);

        m_user_ptr = state.m_device_manager->get_user(res.get_user_id());
        if (m_user_ptr->get_device(res.get_device_id()) != nullptr) {
            hc::util::logger::dbg("device already connected! rejecting...");
            return hc::api::client_packet(hc::api::client_packet::opcode::ERROR, { 0x04 }); // device already connected
        }

        m_device_ptr = m_user_ptr->add_device(res.get_device_id(), shared_from_this());
        m_device_ptr->set_state(initial_state);

        hc::util::logger::dbg("device is powered " + std::string((initial_state.get_power() == hc::api::state::power::ON) ? "on" : "off"));
    } catch(hc::exception& e) {
        hc::util::logger::dbg("failed to validate device ticket: " + std::string(e.what()));
        return err_packet;
    }

    for(auto& x : m_user_ptr->get_associated_handlers()) {
        try {
            x->send_connect_packet(m_device_ptr->get_id(), state_data);
        } catch(hc::exception& e) {
            hc::util::logger::dbg("failed to send connect notification to user: " + std::string(e.what()));
        }
    }

    hc::util::logger::dbg("device authenticated!");

    m_authenticated = true;

    /*hc::util::logger::dbg("starting check connection thread...");
    m_should_check_connection = true;
    m_check_connection_thread = std::thread(&device_handler::check_connection, this);*/

    return hc::api::client_packet(hc::api::client_packet::opcode::AUTHENTICATE, { 0x00 });
}

void device_handler::handle_notification(const hc::api::client_packet& packet) {
    hc::util::logger::dbg("[" + m_device_ptr->get_id() + "] -> *");
    
    hc::api::state state;
    try {
        state.parse(packet.get_data());

        m_device_ptr->set_state(state);

        for (auto& x : m_user_ptr->get_associated_handlers()) {
            x->send_notification_packet(m_device_ptr->get_id(), packet.get_data());
        }
    } catch(hc::exception& e) {
        hc::util::logger::dbg("failed to forward device notification: " + std::string(e.what()));
    }
}

void device_handler::handle_response(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data) {
    if (m_user_queue.empty()) {
        state.m_server->close_connection(conn_ptr);
        throw hc::exception("unrecoverable error; device is misbehaving", "hc::device_handler::handle_response");
    }

    std::weak_ptr<ws_handler> user_handler_hdl = m_user_queue.front();
    
    std::shared_ptr<ws_handler> user_handler_ptr;
    if (!(user_handler_ptr = user_handler_hdl.lock())) {
        hc::util::logger::dbg("failed to forward message, user disconnected");
        return;
    }

    try {
        user_handler_ptr->send_response(data);

        hc::util::logger::dbg("[" + m_device_ptr->get_id() + "] -> [" + m_user_ptr->get_id() + "]");
    } catch(hc::exception& e) {
        hc::util::logger::dbg("failed to forward message to user: " + std::string(e.what()));
    }

    m_user_queue.pop();
}

/*void device_handler::check_connection() {
    while(m_should_check_connection) {
        hc::net::ssl::server_conn_ptr conn_ptr;
        if (!(conn_ptr = m_conn_hdl.lock())) {
            hc::util::logger::err("failed to send test packet, bad connection ptr");
        }

        m_connection_good = false;

        conn_ptr->send({ 0x00 });

        std::this_thread::sleep_for(std::chrono::milliseconds(1500));

        if (!m_connection_good) {
            hc::util::logger::err("connection check failed! device did not respond");
            conn_ptr->close();
        }

        hc::util::logger::dbg("device connection good!");
    }
}*/