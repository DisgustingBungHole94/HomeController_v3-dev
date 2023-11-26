#include "homecontroller/net/ssl/client_connection.h"

namespace hc {
namespace net {
namespace ssl {
    
    bool client_connection::handshake() {
        int res = SSL_connect(m_ssl.get());
        if (res != 1) {
            return false;
        }

        m_ready = true;
        m_closed = false;
        return true;
    }

}
}
}