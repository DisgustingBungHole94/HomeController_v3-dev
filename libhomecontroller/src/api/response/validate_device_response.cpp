#include "homecontroller/api/response/validate_device_response.h"

namespace hc {
namespace api {
    
    void validate_device_response::get_data(json::document& json_doc) {
        m_user_id = json_doc.get_string("userId");
        m_device_id = json_doc.get_string("deviceId");
    }

    std::string validate_device_response::str() {
        std::string json_str =  
            "{"
                "\"success\":true,"
                "\"userId\":\"" + m_user_id + "\","
                "\"deviceId\":\"" + m_device_id + "\""
            "}";
        
        return json_str;
    }

}
}