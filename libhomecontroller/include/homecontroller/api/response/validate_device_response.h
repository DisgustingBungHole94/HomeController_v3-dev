#pragma once

#include "homecontroller/api/response/response.h"

namespace hc {
namespace api {
    
    class validate_device_response : public response {
        public:
            validate_device_response()
                : response(true)
            {}

            ~validate_device_response() {}

            void set_user_id(const std::string& user_id) { m_user_id = user_id; }
            const std::string& get_user_id() const { return m_user_id; }

            void set_device_id(const std::string& device_id) { m_device_id = device_id; }
            const std::string& get_device_id() const { return m_device_id; }

            std::string str() override;

        private:
            void get_data(json::document& json_doc) override;

            std::string m_user_id;
            std::string m_device_id;
    };

}
}