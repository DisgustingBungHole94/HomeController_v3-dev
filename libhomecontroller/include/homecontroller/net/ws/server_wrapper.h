#pragma once

#include "homecontroller/net/ws/wrapper.h"

#include <websocketpp/server.hpp>

namespace hc {
namespace net {
namespace ws {
    
    class server_wrapper : public wrapper<server_config> {
        public:
            server_wrapper() {
                m_ws_server.set_access_channels(websocketpp::log::alevel::none);
                m_ws_server.clear_access_channels(websocketpp::log::alevel::frame_payload);
                m_ws_server.set_error_channels(websocketpp::log::elevel::none);
            }

            ~server_wrapper() {}

            void process_client_handshake(const std::string& upgrade_req_data);

        private:
            void init_ws_conn() override;

            websocketpp::server<server_config> m_ws_server;
    };

}
}
}