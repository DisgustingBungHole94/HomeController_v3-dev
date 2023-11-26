#pragma once

#include "homecontroller/http/generic.h"

#include <string>
#include <map>

namespace hc {
namespace http {

    class response : public generic {
        public:
            response()
                : response("", "")
            {}

            response(const std::string& status, const std::string& body)
                : response(status, body, {})
            {}

            response(const std::string& status, const std::string& body, const std::map<std::string, std::string>& headers) 
                : generic(body, headers), m_status(status)
            {}

            ~response() {}

            void set_status(const std::string& status) { m_status = status; }
            const std::string& get_status() const { return m_status; }

            std::string str() const override;

        private:
            std::string m_status;

    };

}
}