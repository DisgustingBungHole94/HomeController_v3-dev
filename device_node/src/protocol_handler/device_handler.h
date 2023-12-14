#pragma once

#include "../state.h"
#include "protocol_handler.h"

#include <homecontroller/api/client_packet.h>
#include <homecontroller/net/ws/server_wrapper.h>
#include <mutex>
#include <condition_variable>
#include <queue>

class ws_handler;

class device_handler : public protocol_handler, public std::enable_shared_from_this<device_handler> {
    public:
        device_handler(hc::net::ssl::server_conn_ptr conn_ptr)
            : m_conn_hdl(conn_ptr), m_authenticated(false), m_connection_good(false)
        {}

        ~device_handler() {}

        void send_upgrade_response(const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& upgrade_request);

        void on_destroyed(const state& state) override;

        void send_and_forward_response(std::weak_ptr<ws_handler> user_handler_ptr, const std::string& data);

    private:
        void on_data(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr) override;

        void handle_packet(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data, const hc::api::client_packet& packet);

        hc::api::client_packet handle_authenticate(const state& state, const hc::api::client_packet& packet);
        void handle_notification(const hc::api::client_packet& packet);
        void handle_response(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data); 

        bool check_connection() override;
        bool m_connection_good;

        //hc::net::ssl::server_conn_ptr m_conn_ptr;
        hc::net::ssl::server_conn_hdl m_conn_hdl;

        bool m_authenticated;

        //std::string m_user_id;
        //std::string m_device_id;
        std::shared_ptr<user> m_user_ptr;
        std::shared_ptr<device> m_device_ptr;

        std::queue<std::weak_ptr<ws_handler>> m_user_queue;

};