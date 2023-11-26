#pragma once

#include "../state.h"
#include "protocol_handler.h"

#include <homecontroller/net/ws/server_wrapper.h>
#include <homecontroller/api/client_packet.h>

class ws_handler : public protocol_handler, public std::enable_shared_from_this<ws_handler> {
    public:
        ws_handler()
            : m_authenticated(false)
        {}

        ~ws_handler() {}

        void init(const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& upgrade_request);

        void on_destroyed(const state& state) override;

        void send_notification(const std::string& device_id, const std::string& data);

    private:
        void on_data(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data) override;

        bool authenticate(const state& state, const hc::api::client_packet& packet);

        std::shared_ptr<hc::net::ws::server_wrapper> m_ws_wrapper_ptr;

        bool m_authenticated;
        //std::string m_user_id;

        std::shared_ptr<user> m_user_ptr;

};