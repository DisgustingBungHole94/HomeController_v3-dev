#include "homecontroller/api/request_maker.h"

#include "homecontroller/api/info.h"
#include "homecontroller/exception.h"
#include "homecontroller/util/logger.h"

#include <iostream>

namespace hc {
namespace api {
    
    void request_maker::connect() {
        m_http_parser.init(hc::http::parser::type::RESPONSE);
        connect_to_api();
    }

    void request_maker::disconnect() {
        if (m_connected_to_api || m_connected_to_node) {
            m_ssl_client.disconnect();
            m_connected_to_api = false;
            m_connected_to_node = false;
        }
    }

    std::unique_ptr<device> request_maker::login_device(const std::string& username, const std::string& password, const std::string& device_id) {
        if (!m_connected_to_api) {
            connect_to_api();
        }

        login_device_request request;
        request.set_username(username);
        request.set_password(password);
        request.set_device_id(device_id);

        util::logger::dbg("sending login request...");

        login_device_response response;
        make_request(request, response);

        if (!response.get_success()) {
            throw exception("login failed: " + response.get_error_msg(), "hc::api::request_maker::login_device");
        }

        util::logger::dbg("login successful! connecting to node...");

        m_logged_in = true;

        connect_to_node(response.get_node_host(), response.get_node_port());
        
        hc::http::request upgrade_request;
        upgrade_request.set_method("GET");
        upgrade_request.set_url("/");
        upgrade_request.add_header("Connection", "upgrade");
        upgrade_request.add_header("Upgrade", info::DEVICE_UPGRADE_SCHEMA + "/" + info::VERSION);

        hc::http::response upgrade_response = make_http_request(upgrade_request);
        if (upgrade_response.get_status() != "Switching Protocols") {
            throw exception("failed to upgrade connection", "hc::api::request_maker::login_device");
        }

        m_ssl_client.disable_timeout();

        util::logger::dbg("node connection successful!");

        std::unique_ptr<device> device_ptr = std::make_unique<device>(m_ssl_client.get_conn(), response.get_device_id(), response.get_ticket());

        m_ssl_client.reset_conn();
        m_connected_to_node = false;

        return std::move(device_ptr);
    }

    validate_device_response request_maker::validate_device(const std::string& ticket, const std::string& node_secret) {
        if (!m_connected_to_api) {
            connect_to_api();
        }

        validate_request request("/validate_device");
        request.set_ticket(ticket);

        validate_device_response response;
        make_request(request, response, "Type=Node Secret=" + node_secret);

        if (!response.get_success()) {
            throw exception(response.get_error_msg(), "hc::api::request_maker::validate_device");
        }

        return response;
    }

    validate_user_response request_maker::validate_user(const std::string& ticket, const std::string& node_secret) {
        if (!m_connected_to_api) {
            connect_to_api();
        }

        validate_request request("/validate_user");
        request.set_ticket(ticket);

        validate_user_response response;
        make_request(request, response, "Type=Node Secret=" + node_secret);

        if (!response.get_success()) {
            throw exception("validate user failed: " + response.get_error_msg(), "hc::api::request_maker::validate_user");
        }

        return response;
    }

    void request_maker::connect_to_api() {
        if (m_connected_to_node) {
            m_ssl_client.disconnect();
            m_connected_to_node = false;
        }

        m_ssl_client.init();
        m_ssl_client.set_host(hc::api::info::API_HOST);
        m_ssl_client.set_port(hc::api::info::API_PORT);

        m_ssl_client.connect();
        m_connected_to_api = true;
    }

    void request_maker::connect_to_node(const std::string& host, const std::string& port) {
        if (m_connected_to_api) {
            m_ssl_client.disconnect();
            m_connected_to_api = false;
        }

        m_ssl_client.init();
        m_ssl_client.set_host(host);
        m_ssl_client.set_port(port);

        m_ssl_client.connect();
        m_connected_to_node = true;
    }

    void request_maker::make_request(request& request_ref, response& response_ref, const std::string& auth_header_params) {
        http::request http_req;
        http_req.add_header("Authorization", hc::api::info::AUTH_SCHEMA + " " + auth_header_params);
        make_request(http_req, request_ref, response_ref);
    }

    void request_maker::make_request(request& request_ref, response& response_ref) {
        http::request http_req;
        make_request(http_req, request_ref, response_ref);
    }

    void request_maker::make_request(http::request& http_request_ref, request& request_ref, response& response_ref) {
        std::lock_guard<std::mutex> lock(m_mutex);

        http_request_ref.set_method(request_ref.get_method());
        http_request_ref.set_url(request_ref.get_url());
        http_request_ref.set_body(request_ref.str());
        http_request_ref.add_header("Host", info::API_HOST + ":" + info::API_PORT);
        http_request_ref.add_header("Connection", "keep-alive");
        http_request_ref.add_header("Content-Type", "application/json");
        http_request_ref.add_header("Content-Length", std::to_string(http_request_ref.get_body().length()));
        http_request_ref.add_header("Accept", "*/*");

        http::response http_response = make_http_request(http_request_ref);

        response_ref.parse(http_response.get_body());
    }

    http::response request_maker::make_http_request(const http::request& http_request) {        
        std::string request_str = http_request.str();
        std::string data;

        static const int MAX_RETRIES = 3;
        int curr_retries = 0;
        bool success = false;

        do {
            util::logger::dbg("trying api connection... (attempt " + std::to_string(curr_retries + 1) + ")");

            try {
                if (m_ssl_client.get_conn()->is_closed()) {
                    util::logger::dbg("lost connection with api, reconnecting...");
                    m_ssl_client.connect();
                }

                m_ssl_client.get_conn()->send(request_str);
                data = m_ssl_client.get_conn()->recv();
                
                if (!m_ssl_client.get_conn()->is_closed()) {
                    success = true;
                }
            } catch(exception& e) {
                util::logger::dbg("error occurred: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");
            }

            curr_retries++;
        } while(curr_retries < MAX_RETRIES && !success);

        if (!success) {
            throw exception("failed to establish connection with api", "hc::api::request_maker::make_request");
        } else {
            util::logger::dbg("api connection successful");
        }

        /*while(!m_http_parser.parse(data)) {
            try {
                data = m_ssl_client.get_conn()->recv();
            } catch(hc::exception& e) {
                throw exception("failed while receiving chunked message", "hc::api::request_maker::make_request");
            }
        }*/
        m_http_parser.parse(data);
        
        return m_http_parser.get_response();
    }

}
}