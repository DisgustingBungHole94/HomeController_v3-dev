#include "homecontroller/api/response/validate_user_response.h"

namespace hc {
namespace api {
    
    void validate_user_response::get_data(json::document& json_doc) {
        m_user_id = json_doc.get_string("userId");
    }

    std::string validate_user_response::str() {
        std::string json_str =  
            "{"
                "\"success\":true,"
                "\"userId\":\"" + m_user_id + "\""
            "}";
        
        return json_str;
    }

}
}