#include "homecontroller/net/ssl/server_connection.h"
#include "homecontroller/util/logger.h"

namespace hc {
namespace net {
namespace ssl {
    
    bool server_connection::handshake() {
        int res = SSL_accept(m_ssl.get());
        if (res != 1) {
            int err = SSL_get_error(m_ssl.get(), res);
            if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
                return true;
            }

            return false;
        }

        m_closed = false;
        m_ready = true;
        return true;
    }

}
}
}