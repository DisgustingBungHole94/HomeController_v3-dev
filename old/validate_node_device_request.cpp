#include "homecontroller/api/request/validate_node_device_request.h"

#include "homecontroller/util/str.h"
#include "homecontroller/exception.h"

namespace hc {
namespace api {
    
    std::string validate_node_device_request::str() {
        std::string json_str = "{"
                                    "\"device_id\":\"" + m_device_id + "\""
                                "}";

        return json_str;
    }

    void validate_node_device_request::get_data(json::document& json_doc) {
        m_device_id = json_doc.get_string("device_id");
    }

}
}