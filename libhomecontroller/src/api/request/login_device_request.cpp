#include "homecontroller/api/request/login_device_request.h"

namespace hc {
namespace api {
    
    std::string login_device_request::str() {
        std::string json_str =
            "{"
                "\"username\":\"" + m_username + "\","
                "\"password\":\"" + m_password + "\","
                "\"deviceId\":\"" + m_device_id + "\""
            "}";

        return json_str;
    }

    void login_device_request::get_data(json::document& json_doc) {
        m_username = json_doc.get_string("username");
        m_password = json_doc.get_string("password");
        m_device_id = json_doc.get_string("deviceId");
    }

}
}