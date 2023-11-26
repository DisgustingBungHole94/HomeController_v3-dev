#pragma once

#include "homecontroller/api/response/response.h"

namespace hc {
namespace api {
    
    class login_device_response : public response {
        public:
            login_device_response()
                : response(true)
            {}

            ~login_device_response() {}

            void set_user_id(const std::string user_id) { m_user_id = user_id; }
            const std::string& get_user_id() { return m_user_id; }

            void set_device_id(const std::string device_id) { m_device_id = device_id; }
            const std::string& get_device_id() { return m_device_id; }

            void set_node_host(const std::string node_host) { m_node_host = node_host; }
            const std::string& get_node_host() { return m_node_host; }
            
            void set_node_port(const std::string node_port) { m_node_port = node_port; }
            const std::string& get_node_port() { return m_node_port; }

            void set_ticket(const std::string ticket) { m_ticket = ticket; }
            const std::string& get_ticket() { return m_ticket; }

            void set_token(const std::string& token) { m_token = token; }
            const std::string& get_token() const { return m_token; }

            std::string str() override;

        private:
            void get_data(json::document& json_doc) override;

            std::string m_user_id;
            std::string m_device_id;
            std::string m_node_id;
            std::string m_node_host;
            std::string m_node_port;
            std::string m_ticket;
            std::string m_token;
    };

}
}