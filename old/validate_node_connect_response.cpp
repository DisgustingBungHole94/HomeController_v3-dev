#include "homecontroller/api/response/validate_node_connect_response.h"

namespace hc {
namespace api {
    
    void validate_node_connect_response::get_data(json::document& json_doc) {
        m_user_id = json_doc.get_string("user_id");
    }

    std::string validate_node_connect_response::str() {
        std::string json_str =  
            "{"
                "\"success\":true,"
                "\"user_id\":\"" + m_user_id + "\""
            "}";
        
        return json_str;
    }

}
}