#include "homecontroller/net/ws/server_wrapper.h"

#include "homecontroller/exception.h"

#include <iostream>

namespace hc {
namespace net {
namespace ws {

    void server_wrapper::process_client_handshake(std::shared_ptr<ssl::connection> conn_ptr, const std::string& upgrade_req) {
        if (m_tmp_conn_ptr == nullptr) {
            throw hc::exception("no connection being used", "hc::net::ws::wrapper::recv");
        }
        
        m_ws_conn_ptr->read_all(upgrade_req.c_str(), upgrade_req.length());
    }

    void server_wrapper::init_ws_conn() {
        m_ws_conn_ptr = m_ws_server.get_connection();
    }
    
}
}
}