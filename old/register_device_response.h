#pragma once

#include "homecontroller/api/response/response.h"

namespace hc {
namespace api {
    
    class register_device_response : public response {
        public:
            register_device_response()
                : response(true)
            {}

            ~register_device_response() {}

            void set_device_id(const std::string& device_id) { m_device_id = device_id; }
            const std::string& get_device_id() const { return m_device_id; }

            void set_node_host(const std::string& node_host) { m_node_host = node_host; }
            const std::string& get_node_host() const { return m_node_host; }

            void set_node_port(const std::string& node_port) { m_node_port = node_port; }
            const std::string& get_node_port() const { return m_node_port; }

            std::string str() override;

        private:
            void get_data(json::document& json_doc) override;

            std::string m_device_id;

            std::string m_node_host;
            std::string m_node_port;
    };

}
}