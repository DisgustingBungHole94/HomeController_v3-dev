#pragma once

#include "homecontroller/net/ws/wrapper.h"

#include <websocketpp/client.hpp>

namespace hc {
namespace net {
namespace ws {
    
    class client_wrapper : public wrapper<client_config> {
        public:
            client_wrapper() {
                m_ws_client.set_access_channels(websocketpp::log::alevel::none);
                m_ws_client.clear_access_channels(websocketpp::log::alevel::frame_payload);
                m_ws_client.set_error_channels(websocketpp::log::elevel::none);
            }

            ~client_wrapper() {}

            void set_uri(const std::string& uri) { m_uri = uri; }

        private:
            void init_ws_conn() override;

            websocketpp::client<client_config> m_ws_client;

            std::string m_uri;

    };

}
}
}