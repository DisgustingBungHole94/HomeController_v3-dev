#include "homecontroller/api/request/connect_device_request.h"

#include "homecontroller/util/str.h"
#include "homecontroller/exception.h"

namespace hc {
namespace api {
    
    std::string connect_device_request::str() {
        std::string json_str = "{"
                                    "\"device_id\":\"" + m_device_id + "\""
                                "}";

        return json_str;
    }

    void connect_device_request::get_data(json::document& json_doc) {
        m_device_id = json_doc.get_string("device_id");
    }

}
}