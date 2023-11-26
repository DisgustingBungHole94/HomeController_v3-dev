#include "homecontroller/api/request/register_device_request.h"

namespace hc {
namespace api {
    
    std::string register_device_request::str() {
        std::string json_str =
            "{"
                "\"device_type\":\"" + m_device_type + "\","
                "\"device_name\":\"" + m_device_name + "\","
                "\"device_note\":\"" + m_device_note + "\""
            "}";

        return json_str;
    }

    void register_device_request::get_data(json::document& json_doc) {
        m_device_type = json_doc.get_string("device_type");
        m_device_name = json_doc.get_string("device_name");
        m_device_note = json_doc.get_string("device_note");
    }

}
}