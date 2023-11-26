#pragma once

#include "homecontroller/api/request/request.h"

namespace hc {
namespace api {
    
    class validate_node_connect_request : public request {
        public:
            enum class identity {
                DEVICE, USER
            };

            validate_node_connect_request()
                : request("POST", "/validate_node_connect")
            {}

            ~validate_node_connect_request() {}

            void set_identity(identity identity) { m_identity = identity; }
            identity get_identity() const { return m_identity; }

            void set_device_id(const std::string& device_id) { m_device_id = device_id; }
            const std::string& get_device_id() const { return m_device_id; }

            std::string str() override;

        private:
            void get_data(json::document& json_doc) override;

            identity m_identity;
            std::string m_device_id;
    };

}
}