#pragma once

#include "../state.h"
#include "protocol_handler.h"

#include <homecontroller/net/ws/server_wrapper.h>
#include <homecontroller/api/client_packet.h>

class ws_handler : public protocol_handler, public std::enable_shared_from_this<ws_handler> {
    public:
        ws_handler(hc::net::ssl::server_conn_ptr conn_ptr)
            : m_conn_hdl(conn_ptr), m_authenticated(false), m_should_check_connection(false), m_connection_good(false)
        {}

        ~ws_handler() {}

        void send_upgrade_response(const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& upgrade_request);

        void on_destroyed(const state& state) override;

        void send_response(const std::string& data);

        void send_connect_packet(const std::string& device_id, const std::string& data);
        void send_connect_packet(hc::net::ssl::server_conn_ptr conn_ptr, const std::string& device_id, const std::string& data);

        void send_disconnect_packet(const std::string& device_id);
        void send_notification_packet(const std::string& device_id, const std::string& data);

    private:
        void on_data(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr) override;
        void handle_packet(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data, const hc::api::client_packet& packet);

        hc::api::client_packet handle_authenticate(const state& state, hc::net::ssl::server_conn_ptr conn_ptr, const hc::api::client_packet& packet);
        bool send_to_device(const std::string& device_id, const std::string& data);

        void check_connection();
        std::thread m_check_connection_thread;

        bool m_should_check_connection;
        bool m_connection_good;

        hc::net::ssl::server_conn_hdl m_conn_hdl;

        hc::net::ws::server_wrapper m_ws_wrapper;

        bool m_authenticated;

        std::shared_ptr<user> m_user_ptr;

};