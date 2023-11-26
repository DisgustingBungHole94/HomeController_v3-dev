#pragma once

#include <memory>
#include <string>
#include <mutex>
#include <openssl/ssl.h>
#include <openssl/bio.h>

namespace hc {
namespace net {
namespace ssl {

    template<class T> struct deleter_of;
    template<> struct deleter_of<SSL_CTX>   { void operator()(SSL_CTX* p) const     { SSL_CTX_free(p); }};
    template<> struct deleter_of<SSL>       { void operator()(SSL* p) const         { SSL_free(p); }};
    template<> struct deleter_of<BIO>       { void operator()(BIO* p) const         { BIO_free(p); }};
    template<> struct deleter_of<BIO_METHOD>{ void operator()(BIO_METHOD* p) const  { BIO_meth_free(p); }};

    template<class openssl_type>
    using unique_ptr = std::unique_ptr<openssl_type, deleter_of<openssl_type>>;

    class string_bio {
        public:
            string_bio(string_bio&) = delete;
            string_bio& operator=(string_bio&) = delete;

            explicit string_bio();

            BIO* bio() { return m_bio.get(); }
            const std::string& str() { return m_str; }

        private:
            std::string m_str;

            unique_ptr<BIO_METHOD> m_methods;
            unique_ptr<BIO> m_bio;
    };

    extern std::string error_str();

    class connection {
        public:
            connection(unique_ptr<SSL> ssl)
                : m_ssl(std::move(ssl)), m_closed(true)
            {}

            ~connection() {}

            virtual bool handshake();

            std::string recv();
            void send(const std::string& data);

            void close();

            int get_socket() { return SSL_get_fd(m_ssl.get()); }
            bool is_closed();

        protected:
            unique_ptr<SSL> m_ssl;

            bool m_closed;

            // thread safety
            std::mutex m_mutex;
    };

}
}
}