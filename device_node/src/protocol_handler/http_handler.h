#pragma once

#include "../state.h"
#include "protocol_handler.h"

#include <homecontroller/http/parser.h>

class http_handler : public protocol_handler {
    public:
        http_handler() {}
        ~http_handler() {}

        void init();

    private:
        void on_data(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data) override;

        hc::http::response process_request(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data, const hc::http::request& request);

        bool handle_upgrade(const hc::net::ssl::server_conn_ptr& conn_ptr, const hc::http::request& request, const std::string& data, hc::http::response& response_ref);
        
        /*hc::http::response handle_connect(const std::vector<std::string>& path, const state& state, const hc::http::request& request);
        bool parse_client_auth_header(const hc::http::request& request, std::string& client_token_ref);
        
        bool validate_node_user(const state& state, const std::string& client_token, hc::http::response& response_ref, std::string& user_id_ref);
        bool validate_node_device(const state& state, const std::string& client_token, hc::http::response& response_ref, std::string& user_id_ref, const std::string& device_id);

        bool validate_node_connect(hc::api::validate_node_connect_response& api_response, hc::http::response& response_ref, std::string& user_id_ref);*/

        hc::http::parser m_http_parser;

        bool m_keep_alive;
    
};