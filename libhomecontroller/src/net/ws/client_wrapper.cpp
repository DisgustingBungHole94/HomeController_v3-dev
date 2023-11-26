#include "homecontroller/net/ws/client_wrapper.h"

#include "homecontroller/exception.h"

namespace hc {
namespace net {
namespace ws {
    
    void client_wrapper::init_ws_conn() {
        std::error_code ec;
        m_ws_conn_ptr = m_ws_client.get_connection(m_uri, ec);

        if (ec) {
            throw exception("failed to wrap connection: " + ec.message(), "hc::net::ws::client_wrapper::init_ws_conn");
        }
    }

}
}
}