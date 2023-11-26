#pragma once

#include "homecontroller/api/response/response.h"

namespace hc {
namespace api {
    
    class validate_node_connect_response : public response {
        public:
            validate_node_connect_response()
                : response(true)
            {}

            ~validate_node_connect_response() {}

            void set_user_id(const std::string& user_id) { m_user_id = user_id; }
            const std::string& get_user_id() const { return m_user_id; }

            std::string str() override;

        private:
            void get_data(json::document& json_doc) override;

            std::string m_user_id;
    };

}
}