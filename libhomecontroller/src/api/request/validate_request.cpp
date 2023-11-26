#include "homecontroller/api/request/validate_request.h"

namespace hc {
namespace api {
    
    std::string validate_request::str() {
        std::string json_str =
            "{"
                "\"ticket\":\"" + m_ticket + "\""
            "}";

        return json_str;
    }

    void validate_request::get_data(json::document& json_doc) {
        m_ticket = json_doc.get_string("ticket");
    }

}
}