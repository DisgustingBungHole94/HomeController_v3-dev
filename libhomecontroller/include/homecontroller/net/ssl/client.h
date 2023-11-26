#pragma once

#include "homecontroller/net/ssl/client_connection.h"

#include <string>

namespace hc {
namespace net {
namespace ssl {
    
    class client {
        public:
            client() {}
            ~client() {}

            void init();

            void connect();
            void disconnect();

            void set_host(const std::string& host) { m_host = host; }
            const std::string& get_host() { return m_host; }

            void set_port(std::string port) { m_port = port; }
            const std::string& get_port() { return m_port; }

            client_conn_ptr get_conn() { return m_conn_ptr; }
            void reset_conn() { m_conn_ptr.reset(); }

            void disable_timeout();

        private:
            unique_ptr<SSL_CTX> m_ssl_ctx;

            std::string m_host;
            std::string m_port;

            client_conn_ptr m_conn_ptr;

    };

}
}
}