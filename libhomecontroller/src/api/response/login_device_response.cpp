#include "homecontroller/api/response/login_device_response.h"

#include <iostream>

namespace hc {
namespace api {
    
    void login_device_response::get_data(json::document& json_doc) {
        m_user_id = json_doc.get_string("userId");
        m_device_id = json_doc.get_string("deviceId");

        json::document node_doc;
        json_doc.get_object("node", node_doc);

        m_node_id = node_doc.get_string("id");
        m_node_host = node_doc.get_string("host");
        m_node_port = node_doc.get_string("port");

        m_ticket = json_doc.get_string("ticket");
        m_token = json_doc.get_string("token");
    }

    std::string login_device_response::str() {
        std::string json_str =  
            "{"
                "\"success\":true,"
                "\"userId\":\"" + m_user_id + "\","
                "\"deviceId\":\"" + m_device_id + "\","
                "\"node\":{\""
                    "\"id\":\"" + m_node_id + "\","
                    "\"host\":\"" + m_node_host + "\","
                    "\"port\":\"" + m_node_port + "\""
                "},"
                "\"ticket\":\"" + m_ticket + "\","
                "\"token\":\"" + m_token + "\""
            "}";
        
        return json_str;
    }

}
}