#pragma once

#include "homecontroller/http/generic.h"

#include <string>
#include <map>

namespace hc {
namespace http {

    class request : public generic {
        public:
            request()
                : request("", "", "")
            {}

            request(const std::string& method, const std::string& url, const std::string& body)
                : request(method, url, body, {})
            {}

            request(const std::string& method, const std::string& url, const std::string& body, const std::map<std::string, std::string>& headers) 
                : generic(body, headers), m_method(method), m_url(url)
            {}

            ~request() {}

            void set_method(const std::string& method) { m_method = method; }
            const std::string& get_method() const { return m_method; }

            void set_url(const std::string& url) { m_url = url; }
            const std::string& get_url() const { return m_url; }

            std::string str() const override;

        private:
            std::string m_method;
            std::string m_url;

    };

}
}