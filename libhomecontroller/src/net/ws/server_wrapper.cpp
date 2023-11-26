#include "homecontroller/net/ws/server_wrapper.h"

namespace hc {
namespace net {
namespace ws {

    void server_wrapper::process_client_handshake(const std::string& upgrade_req) {
        m_ws_conn_ptr->read_all(upgrade_req.c_str(), upgrade_req.length());
    }

    void server_wrapper::init_ws_conn() {
        m_ws_conn_ptr = m_ws_server.get_connection();
    }
    
}
}
}