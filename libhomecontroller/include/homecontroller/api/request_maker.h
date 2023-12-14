#pragma once

#include "homecontroller/net/ssl/client.h"
#include "homecontroller/http/request.h"
#include "homecontroller/http/parser.h"
#include "homecontroller/api/request/login_device_request.h"
#include "homecontroller/api/response/login_device_response.h"
#include "homecontroller/api/request/validate_request.h"
#include "homecontroller/api/response/validate_device_response.h"
#include "homecontroller/api/response/validate_user_response.h"
#include "homecontroller/api/device.h"

#include <mutex>

namespace hc {
namespace api {

    class request_maker {
        public:
            request_maker() 
                : m_connected_to_api(false), m_connected_to_node(false)
            {}

            ~request_maker() {}

            void connect();
            void disconnect();

            std::unique_ptr<device> login_device(const std::string& username, const std::string& password, const std::string& device_id);

            //std::unique_ptr<device> upgrade_to_device(const std::string& device_id, const std::string& node_host, const std::string& node_port, const std::string& ticket);

            /*validate_node_connect_response validate_node_user(const std::string& client_token, const std::string& node_secret);
            validate_node_connect_response validate_node_device(const std::string& client_token, const std::string& node_secret, const std::string& device_id);*/

            validate_device_response validate_device(const std::string& ticket, const std::string& node_secret);
            validate_user_response validate_user(const std::string& ticket, const std::string& node_secret);

            bool is_connected() { return m_connected_to_api || m_connected_to_node; }

        private:
            void connect_to_api();
            void connect_to_node(const std::string& host, const std::string& port);

            void make_request(request& request_ref, response& response_ref);
            void make_request(request& request_ref, response& response_ref, const std::string& auth_header_params);

            void make_request(http::request& http_request_ref, request& request_ref, response& response_ref);

            http::response make_http_request(const http::request& http_request);

            //validate_node_connect_response validate_node_connect(const std::string& client_token, const std::string& node_secret, request& request_ref);

            net::ssl::client m_ssl_client;

            bool m_connected_to_api;
            bool m_connected_to_node;

            http::parser m_http_parser;

            bool m_logged_in;

            std::mutex m_mutex;
    };

}
}