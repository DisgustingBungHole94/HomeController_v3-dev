#pragma once

#include "homecontroller/api/response/response.h"

namespace hc {
namespace api {
    
    class connect_response : public response {
        public:
            connect_response()
                : response(true)
            {}

            ~connect_response() {}

            void set_ticket(const std::string& ticket) { m_ticket = ticket; }
            const std::string& get_ticket() const { return m_ticket; }

            std::string str() override;

        private:
            void get_data(json::document& json_doc) override;

            std::string m_ticket;
    };

}
}