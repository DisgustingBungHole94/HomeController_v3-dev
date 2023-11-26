#include "homecontroller/api/response/connect_response.h"

namespace hc {
namespace api {
    
    void connect_response::get_data(json::document& json_doc) {
        m_ticket = json_doc.get_string("ticket");
    }

    std::string connect_response::str() {
        std::string json_str =  
            "{"
                "\"success\":true,"
                "\"ticket\":\"" + m_ticket + "\""
            "}";
        
        return json_str;
    }

}
}