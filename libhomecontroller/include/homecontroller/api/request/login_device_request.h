#pragma once

#include "homecontroller/api/request/request.h"

namespace hc {
namespace api {
    
    class login_device_request : public request {
        public:
            login_device_request()
                : request("POST", "/login/device")
            {}

            ~login_device_request() {}

            void set_username(const std::string& username) { m_username = username; }
            const std::string& get_username() const { return m_username; }

            void set_password(const std::string& password) { m_password = password; }
            const std::string& get_password() const { return m_password; }

            void set_device_id(const std::string& device_id) { m_device_id = device_id; }
            const std::string& get_device_id() const { return m_device_id; }

            std::string str() override;

        private:
            void get_data(json::document& json_doc) override;

            std::string m_username;
            std::string m_password;
            std::string m_device_id;
    };

}
}