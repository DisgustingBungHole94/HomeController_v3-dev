#pragma once

#include "homecontroller/api/request/request.h"

namespace hc {
namespace api {
    
    class validate_request : public request {
        public:
            validate_request(const std::string& url)
                : request("POST", url)
            {}

            ~validate_request() {}

            void set_ticket(const std::string& ticket) { m_ticket = ticket; }
            const std::string& get_ticket() const { return m_ticket; }

            std::string str() override;

        private:
            void get_data(json::document& json_doc) override;

            std::string m_ticket;
    };

}
}