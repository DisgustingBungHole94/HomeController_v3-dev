#pragma once

#include <string>
#include <map>

namespace hc {
namespace http {
    
    class generic {
        public:
            generic(const std::string& body, const std::map<std::string, std::string>& headers) 
                : m_body(body), m_headers(headers), m_should_upgrade(false)
            {}
            
            ~generic() {}

            void set_body(const std::string& body) { m_body = body; }
            const std::string& get_body() const { return m_body; }

            void set_headers(const std::map<std::string, std::string>& headers) { m_headers = headers; }

            void add_header(const std::string& name, const std::string& value);
            bool get_header(const std::string& name, std::string& value_ref) const;

            void set_should_upgrade(bool should_upgrade) { m_should_upgrade = should_upgrade; }
            bool get_should_upgrade() const { return m_should_upgrade; }

            virtual std::string str() const;

        protected:
            std::string m_body;
            std::map<std::string, std::string> m_headers;
            bool m_should_upgrade;
    };

}
}