#include "homecontroller/net/ssl/connection.h"

#include "homecontroller/exception.h"
#include "homecontroller/util/logger.h"

#include <vector>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

#include <openssl/err.h>

namespace hc {
namespace net {
namespace ssl {
    
    string_bio::string_bio() {
        m_methods.reset(BIO_meth_new(BIO_TYPE_SOURCE_SINK, "string_bio"));
        if (m_methods == nullptr) {
            throw exception("failed to create methods for string_bio", "hc::net::ssl::string_bio::string_bio");
        }

        BIO_meth_set_write(m_methods.get(), [](BIO* bio, const char* data, int len) -> int {
            std::string* str = reinterpret_cast<std::string*>(BIO_get_data(bio));
            str->append(data, len);

            return len;
        });

        m_bio.reset(BIO_new(m_methods.get()));
        if (m_bio == nullptr) {
            throw exception("failed to create new bio for string_bio", "hc::net::ssl::string_bio::string_bio");
        }

        BIO_set_data(m_bio.get(), &m_str);
        BIO_set_init(m_bio.get(), 1);
    }

    std::string error_str() {
        string_bio bio;
        ERR_print_errors(bio.bio());
        
        return bio.str();
    }

    bool connection::handshake() {
        return false;
    }

    std::string connection::recv() {
        //std::lock_guard<std::mutex> lock(m_mutex);

        if (m_closed) {
            throw exception("socket is closed", "hc::net::ssl::connection::recv");
        }

        int num_bytes = 0;
        std::string data;

        static const unsigned int MAX_BUF_LEN = 4096;
        std::vector<unsigned char> buf(MAX_BUF_LEN);

        do {
            num_bytes = SSL_read(m_ssl.get(), &buf[0], buf.size());

            if (num_bytes < 0) {
                int err = SSL_get_error(m_ssl.get(), num_bytes);
                switch(err) {
                    case SSL_ERROR_SYSCALL:
                        if (!m_closed) {
                            m_closed = true;
                            util::logger::err("underlying socket error (recv, " + std::to_string(errno) + ")");
                        }
                        return "";
                    case SSL_ERROR_WANT_READ:
                        return recv();
                    default:
                        util::logger::err("openssl error: " + error_str());
                }

                throw exception("failed to receive bytes", "hc::net::ssl::connection::recv");
            } else if (num_bytes == 0) {
                m_closed = true;
                return "";
            }

            buf.resize(num_bytes);
            data.append(buf.cbegin(), buf.cend());
        } while(num_bytes >= MAX_BUF_LEN);

        return data;
    }

    void connection::send(const std::string& data) {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (m_closed) {
            throw exception("socket is closed", "hc::net::ssl::connection::send");
        }

        int res = SSL_write(m_ssl.get(), data.c_str(), data.size());
        std::cout << "res: " << res << std::endl;

        if (res <= 0) {
            int err = SSL_get_error(m_ssl.get(), res);
            std::cout << "err: " << err << std::endl;
            
            switch(err) {
                case SSL_ERROR_SYSCALL:
                    if (!m_closed) {
                        m_closed = true;
                        util::logger::err("underlying socket error (send, " + std::to_string(errno) + ")");
                    }
                    return;
                case SSL_ERROR_WANT_WRITE:
                    send(data);
                    break;
                default:
                    util::logger::err("openssl error: " + error_str());
                    break;
            }

            throw exception("failed to send bytes", "hc::net::ssl::connection::send");
        }
    }

    void connection::close() {
        //std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_closed) {
            m_closed = true;

            SSL_shutdown(m_ssl.get());

            shutdown(get_socket(), SHUT_RDWR);
        }
        ::close(get_socket());
    }

    bool connection::is_closed() {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_closed;
    }

}
}
}