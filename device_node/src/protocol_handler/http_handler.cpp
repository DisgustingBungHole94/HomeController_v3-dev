#include "http_handler.h"

#include "ws_handler.h"
#include "device_handler.h"

#include <homecontroller/util/logger.h>
#include <homecontroller/exception.h>
#include <homecontroller/util/str.h>
#include <homecontroller/http/common.h>
#include <homecontroller/net/ssl/client.h>
#include <homecontroller/api/info.h>

void http_handler::init() {
    m_http_parser.init(hc::http::parser::type::REQUEST);
}

void http_handler::on_data(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data) {
    try {
        if (!m_http_parser.parse(data)) {
            hc::util::logger::dbg("partial http request processed, awaiting more data...");
            return;
        }
    } catch(hc::exception& e) {
        hc::util::logger::err("failed to parse http request: " + std::string(e.what()));
        state.m_server->close_connection(conn_ptr);
        return;
    }

    hc::http::request req = m_http_parser.get_request();
    hc::util::logger::log("[" + conn_ptr->get_ip() + "] -> " + req.get_url());

    hc::http::response res = process_request(state, conn_ptr, data, req);
    
    if (m_should_upgrade_protocol) {
        return;
    }

    if (m_keep_alive) {
        res.add_header("Connection", "keep-alive");
    } else {
        res.add_header("Connection", "close");
    }
    res.add_header("Server", hc::api::info::NAME + "/" + hc::api::info::VERSION + " (" + hc::api::info::OS + ")");
    res.add_header("Access-Control-Allow-Origin", "*");
    res.add_header("Content-Length", std::to_string(res.get_body().length()));

    conn_ptr->send(res.str());
    if (!m_keep_alive) {
        state.m_server->close_connection(conn_ptr);
    }

    hc::util::logger::log(res.get_status() + " -> [" + conn_ptr->get_ip() + "]");
}

hc::http::response http_handler::process_request(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data, const hc::http::request& request) {
    hc::http::response response;

    if (request.get_should_upgrade()) {
        if (!handle_upgrade(conn_ptr, request, data, response)) {
            return response;
        }

        m_should_upgrade_protocol = true;
        return response;
    }

    std::string connection_type;
    if (request.get_header("connection", connection_type) && hc::util::str::to_lower_case(connection_type) == "keep-alive") {
        hc::util::logger::dbg("connection will be kept alive");
        m_keep_alive = true;
    } else {
        hc::util::logger::dbg("connection will be closed");
        m_keep_alive = false;
    }

    std::vector<std::string> split_path = hc::util::str::split_string(request.get_url(), '/');
    if (split_path.size() < 2) {
        return hc::http::common::not_found();
    }

    if (split_path[1] == "test") {
        response = hc::http::response("200 OK", "{\"success\":true}");
    }

    else {
        response = hc::http::common::not_found();
    }

    return response;
}

bool http_handler::handle_upgrade(const hc::net::ssl::server_conn_ptr& conn_ptr, const hc::http::request& request, const std::string& data, hc::http::response& response_ref) {
    hc::util::logger::dbg("received upgrade request");

    bool upgrade_failed = false;

    std::string upgrade_type;
    if (!request.get_header("upgrade", upgrade_type)) {
        hc::util::logger::dbg("request missing upgrade header");
        response_ref = hc::http::common::bad_request();
        return false;
    }

    if (upgrade_type == "websocket") {
        hc::util::logger::dbg("upgrade HTTP -> WebSocket");

        std::shared_ptr<ws_handler> new_protocol = std::make_shared<ws_handler>();
        new_protocol->init(conn_ptr, data); // wrap connection

        m_new_protocol = std::move(new_protocol);
    } 
    
    else if (upgrade_type == hc::api::info::DEVICE_UPGRADE_SCHEMA + "/" + hc::api::info::VERSION) {
        hc::util::logger::dbg("upgrade HTTP -> " + hc::api::info::DEVICE_UPGRADE_SCHEMA + "/" + hc::api::info::VERSION);

        std::shared_ptr<device_handler> new_protocol = std::make_shared<device_handler>();
        new_protocol->init(conn_ptr, data);

        m_new_protocol = std::move(new_protocol);
    }

    else {
        hc::util::logger::dbg("request specified unsupported protocol for upgrade");
        response_ref = hc::http::common::bad_request();
        return false;
    }

    return true;
}