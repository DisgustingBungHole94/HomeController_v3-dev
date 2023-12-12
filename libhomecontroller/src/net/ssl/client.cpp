#include "homecontroller/net/ssl/client.h"

#include "homecontroller/exception.h"

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

namespace hc {
namespace net {
namespace ssl {
    
    void client::init() {
        const SSL_METHOD* method = TLS_client_method();

        m_ssl_ctx.reset(SSL_CTX_new(method));
        if (m_ssl_ctx == nullptr) {
            throw exception("failed to create ssl context", "hc::net::ssl::client::init");
        }
    }

    void client::connect() {
        if (m_conn_ptr && !m_conn_ptr->is_closed()) {
            m_conn_ptr->close();
            m_conn_ptr.reset();
        }

        unique_ptr<SSL> ssl(SSL_new(m_ssl_ctx.get()));
        if (ssl == nullptr) {
            throw exception("failed to create ssl for client", "hc::net::ssl::client::connect");
        }

        addrinfo hints = {};
        addrinfo* addrs;

        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        int err = getaddrinfo(m_host.c_str(), m_port.c_str(), &hints, &addrs);
        if (err != 0) {
            throw exception("failed to resolve host", "hc::net::ssl::client::connect");
        }

        int sd;

        for (addrinfo* addr = addrs; addr != nullptr; addr = addr->ai_next) {
            sd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
            if (sd == -1) {
                err = errno;
                break;
            }

            timeval tv;
            tv.tv_sec = 5;
            tv.tv_usec = 0;
            setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

            if (::connect(sd, addr->ai_addr, addr->ai_addrlen) == 0) {
                break;
            }

            err = errno;
            
            ::close(sd);
            sd = -1;
        }

        freeaddrinfo(addrs);

        if (sd == -1) {
            throw exception("failed to connect to server (" + std::to_string(errno) + ")", "hc::net::ssl::client::run");
        }

        SSL_set_fd(ssl.get(), sd);

        // verify certificate
        if (!X509_VERIFY_PARAM_set1_host(SSL_get0_param(ssl.get()), m_host.c_str(), m_host.length())) {
            throw exception("failed to set cert hostname", "hc::net::ssl::client::run");
        }

        //SSL_set_verify(ssl.get(), SSL_VERIFY_PEER, 0);

        client_conn_ptr client_conn = std::make_shared<client_connection>(std::move(ssl));
        if (!client_conn->handshake()) {
            client_conn->close();
            throw exception("server handshake failed: " + error_str(), "hc::net::ssl::client::run");
        }

        /*X509* cert = SSL_get_peer_certificate(ssl.get());
        if (cert == nullptr) {
            client_conn->close();
            throw exception("server did not provide certificate", "hc::net::ssl::client::run");
        }

        X509_free(cert);

        int res = SSL_get_verify_result(ssl.get());
        if (res != X509_V_OK) {
            client_conn->close();
            throw exception("failed to verify server certificate", "hc::net::ssl::client::run");
        }*/

        m_conn_ptr = client_conn;
    }

    void client::disconnect() {
        m_conn_ptr->close();
    }

    void client::disable_timeout() {
        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        setsockopt(m_conn_ptr->get_socket(), SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    }

}
}
}