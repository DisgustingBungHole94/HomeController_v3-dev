#pragma once

#include "homecontroller/api/message.h"

namespace hc {
namespace api {
    
    class request : public message {
        public:
            request(const std::string& method, const std::string& url) 
                : m_method(method), m_url(url)
            {}

            ~request() {}

            void parse(const std::string& data) override;

            const std::string& get_error_msg() const { return m_error_msg; }

            const std::string& get_method() const { return m_method; }
            const std::string& get_url() const { return m_url; }

        private:
            std::string m_error_msg;

            std::string m_method;
            std::string m_url;
    };

}
}