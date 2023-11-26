#pragma once

#include "../state.h"
#include "protocol_handler.h"

#include <homecontroller/api/client_packet.h>
#include <homecontroller/net/ws/server_wrapper.h>
#include <mutex>
#include <condition_variable>
#include <queue>

class device_handler : public protocol_handler, public std::enable_shared_from_this<device_handler> {
    public:
        device_handler()
            : m_authenticated(false)
        {}

        ~device_handler() {}

        void init(const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& upgrade_request);

        void on_destroyed(const state& state) override;

        void send_and_forward_response(std::weak_ptr<hc::net::ws::server_wrapper> user_conn_hdl, const std::string& data);

    private:
        void on_data(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data) override;

        bool authenticate(const state& state, const hc::api::client_packet& packet);

        hc::net::ssl::server_conn_ptr m_conn_ptr;

        bool m_authenticated;

        //std::string m_user_id;
        //std::string m_device_id;
        std::shared_ptr<user> m_user_ptr;
        std::shared_ptr<device> m_device_ptr;

        std::queue<std::weak_ptr<hc::net::ws::server_wrapper>> m_user_message_queue;

};