#pragma once

#include "connection.h"

namespace hc {
namespace net {
namespace ssl {
    
    class server_connection : public connection {
        public:
            server_connection(unique_ptr<SSL> ssl)
                : connection(std::move(ssl)), m_ready(false), m_timeout_enabled(true)
            {}

            ~server_connection() {}

            bool handshake(); 

            void set_ip(const std::string& ip) { m_ip = ip; }
            const std::string& get_ip() { return m_ip; }

            void set_ready(bool ready) { m_ready = ready; }
            bool is_ready() { return m_ready; }

        private:
            std::string m_ip;

            // has handshake been performed?
            bool m_ready;

            bool m_timeout_enabled;
    };

    typedef std::weak_ptr<server_connection> server_conn_hdl;
    typedef std::shared_ptr<server_connection> server_conn_ptr;

}
}
}