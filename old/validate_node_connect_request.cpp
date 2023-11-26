#include "homecontroller/api/request/validate_node_connect_request.h"

#include "homecontroller/util/str.h"
#include "homecontroller/exception.h"

namespace hc {
namespace api {
    
    std::string validate_node_connect_request::str() {
        std::string json_str = "{";
        
        switch(m_identity) {
            case identity::DEVICE:
                json_str += "\"identity\":\"device\","
                            "\"device_id\":\"" + m_device_id + "\"}";
                break;
            case identity::USER:
                json_str += "\"identity\":\"user\"}";
                break;
        }

        return json_str;
    }

    void validate_node_connect_request::get_data(json::document& json_doc) {
        std::string identity_str = hc::util::str::to_lower_case(json_doc.get_string("identity"));
        if (identity_str == "device") {
            m_identity = identity::DEVICE;
            m_device_id = json_doc.get_string("device_id");
        } else if (identity_str == "user") {
            m_identity == identity::USER;
        } else {
            throw exception("unknown identity specified", "hc::api::validate_node_connect_request::get_data");
        }
    }

}
}