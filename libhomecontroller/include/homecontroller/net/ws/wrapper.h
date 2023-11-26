#pragma once

#include "homecontroller/net/ssl/connection.h"

#include <websocketpp/connection.hpp>
#include <websocketpp/config/core.hpp>
#include <websocketpp/config/core_client.hpp>

namespace hc {
namespace net {
namespace ws {

    struct server_config : public websocketpp::config::core {
        typedef websocketpp::transport::iostream::endpoint<websocketpp::config::core::transport_config> transport_type;
    };

    struct client_config : public websocketpp::config::core_client {
        typedef websocketpp::transport::iostream::endpoint<websocketpp::config::core_client::transport_config> transport_type;
    };

    template<typename config_type>
    class wrapper {
        public:
            wrapper() 
                : m_closed(true)
            {}

            ~wrapper() {}

            void wrap(std::shared_ptr<ssl::connection> conn_ptr);

            void send(const std::string& data);

            void recv();
            void recv_external(const std::string& data); // receive bytes from external source

            void close();
            bool is_closed() { return m_closed; }

            std::string get_last_message() { return m_last_message; }

        private:
            virtual void init_ws_conn();

            std::error_code on_vector_write(websocketpp::connection_hdl hdl, std::vector<websocketpp::transport::buffer> const& bufs);
            std::error_code on_write(websocketpp::connection_hdl hdl, const char* data, std::size_t len);

            void on_open(websocketpp::connection_hdl ws_conn_hdl);
            void on_fail(websocketpp::connection_hdl ws_conn_hdl);
            void on_close(websocketpp::connection_hdl ws_conn_hdl);
            void on_message(websocketpp::connection_hdl hdl, std::shared_ptr<websocketpp::config::core::message_type> msg);

            std::weak_ptr<ssl::connection> m_conn_hdl;
            
            bool m_closed;
            std::string m_last_message;

        protected:
            std::shared_ptr<websocketpp::connection<config_type>> m_ws_conn_ptr;

    };

}
}
}