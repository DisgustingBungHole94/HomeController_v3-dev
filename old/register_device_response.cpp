#include "homecontroller/api/response/register_device_response.h"

namespace hc {
namespace api {
    
    void register_device_response::get_data(json::document& json_doc) {
        m_device_id = json_doc.get_string("deviceId");
        m_node_host = json_doc.get_string("nodeHost");
        m_node_port = json_doc.get_string("nodePort");
    }

    std::string register_device_response::str() {
        std::string json_str =  
            "{"
                "\"success\":true,"
                "\"deviceId\":\"" + m_device_id + "\","
                "\"nodeHost\":\"" + m_node_host + "\","
                "\"nodePort\":\"" + m_node_port + "\""
            "}";
        
        return json_str;
    }

}
}