#pragma once

#include "homecontroller/net/ssl/connection.h"

namespace hc {
namespace net {
namespace ssl {
    
    class client_connection : public connection {
        public:
            client_connection(unique_ptr<SSL> ssl) 
                : connection(std::move(ssl)), m_ready(false)
            {}

            ~client_connection() {}

            bool handshake();

            void set_ready(bool ready) { m_ready = ready; }
            bool is_ready() { return m_ready; }
 
        private:
            bool m_ready;

    };

    typedef std::weak_ptr<client_connection> client_conn_hdl;
    typedef std::shared_ptr<client_connection> client_conn_ptr;

}
}
}