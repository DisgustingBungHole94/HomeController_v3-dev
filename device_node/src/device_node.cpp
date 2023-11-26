#include "device_node.h"

#include <homecontroller/util/logger.h>
#include <homecontroller/exception.h>
#include <homecontroller/json/document.h>
#include <homecontroller/api/info.h>

#include <fstream>
#include <sstream>
#include <iostream>

bool device_node::start() {
    hc::util::logger::log(hc::api::info::NAME + " v" + hc::api::info::VERSION + " by Josh Dittmer");
    hc::util::logger::log("running on " + hc::api::info::OS);

    m_state.m_server = std::make_shared<hc::net::ssl::server>();

    m_state.m_server->set_on_connect_callback(std::bind(&device_node::on_connect, this, std::placeholders::_1));
    m_state.m_server->set_on_data_callback(std::bind(&device_node::on_data, this, std::placeholders::_1));
    m_state.m_server->set_on_disconnect_callback(std::bind(&device_node::on_disconnect, this, std::placeholders::_1));

    m_state.m_api_request_maker = std::make_shared<hc::api::request_maker>();

    m_state.m_device_manager = std::make_shared<device_manager>();

    std::ifstream conf_file("./conf/device_node.json");
    if (!conf_file.good()) {
        hc::util::logger::csh("failed to open config file");
        return false;
    }

    std::ostringstream conf_buf;
    conf_buf << conf_file.rdbuf();

    hc::json::document conf_doc;

    try {
        conf_doc.parse(conf_buf.str());

        m_state.m_secret = conf_doc.get_string("secret");

        m_config.m_port = conf_doc.get_int("server_port");
        m_config.m_tls_cert_file = conf_doc.get_string("tls_cert_file");
        m_config.m_tls_priv_key_file = conf_doc.get_string("tls_priv_key_file");

        m_config.m_max_connections = conf_doc.get_int("max_conns");
        m_config.m_connection_expire_time = conf_doc.get_int("conn_expire_time");

        m_config.m_multithreaded = conf_doc.get_bool("multithreaded");
        if (m_config.m_multithreaded) {
            hc::util::logger::log("multithreading enabled");
        }

        m_config.m_debug_mode = conf_doc.get_bool("debug_mode");
        if (m_config.m_debug_mode) {
            hc::util::logger::enable_debug();
            hc::util::logger::dbg("debug mode enabled");
        }
    } catch(hc::exception& e) {
        hc::util::logger::csh("failed to load config file: " + std::string(e.what()));
        return false;
    }

    try {
        // connect to api
        m_state.m_api_request_maker->connect();

        // init ssl server
        m_state.m_server->init(m_config.m_port, m_config.m_tls_cert_file, m_config.m_tls_priv_key_file, m_config.m_connection_expire_time);

        // init thread pool
        if (m_config.m_multithreaded)
            m_thread_pool.start(m_config.m_max_connections);
    } catch(hc::exception& e) {
        hc::util::logger::csh("failed to intialize: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");
        return false;
    }

    m_state.m_operation = state::operation::RUNNING;
    hc::util::logger::log("successfully initialized");

    // start ssl server
    std::thread server_thread(&hc::net::ssl::server::run, m_state.m_server, m_config.m_max_connections);

    // main loop for command line input
    loop();

    // wait for server thread to close
    server_thread.join();

    m_state.m_server.reset();

    return true;
}

void device_node::shutdown() {
    if (m_state.m_operation != state::operation::RUNNING) {
        hc::util::logger::err("early shutdown warning: please wait until init is finished");
        return;
    }

    hc::util::logger::log("shutting down...");

    // exit command line input loop
    m_state.m_operation = state::operation::STOPPED;
}

void device_node::on_connect(hc::net::ssl::server_conn_hdl conn_hdl) {
    std::shared_ptr<http_handler> handler = std::make_shared<http_handler>();
    handler->init();

    m_conns.insert(std::make_pair(conn_hdl, handler));

    hc::util::logger::dbg("created http handler for client");
}

void device_node::on_data(hc::net::ssl::server_conn_hdl conn_hdl) {
    hc::net::ssl::server_conn_ptr conn_ptr;
    if (!(conn_ptr = conn_hdl.lock())) {
        hc::util::logger::err("bad connection pointer");
        return;
    }

    std::string data = conn_ptr->recv();

    if (m_config.m_multithreaded) {
        m_thread_pool.add_job([=]() {
            client_job(conn_ptr, data);
        });
    }

    else {
        client_job(conn_ptr, data);
    }
}

void device_node::on_disconnect(hc::net::ssl::server_conn_hdl conn_hdl) {\
    hc::net::ssl::server_conn_ptr conn_ptr;
    if (!(conn_ptr = conn_hdl.lock())) {
        hc::util::logger::err("bad connection pointer");
        return;
    }

    if (!conn_ptr->is_ready()) {
        return;
    }

    auto mit = m_conns.find(conn_hdl);
    if (mit == m_conns.end()) {
        hc::util::logger::err("connection has no associated handler, this shouldn't happen");
        return;
    }

    mit->second->set_destroyed(true);
    mit->second->on_destroyed(m_state);

    m_conns.erase(mit);

    hc::util::logger::dbg("client handler destroyed");
}

void device_node::loop() {
    while(m_state.m_operation == state::operation::RUNNING) {
        /*std::string cmd;
        std::getline(std::cin, cmd);

        if (cmd == "stop") {
            shutdown();
        }*/
    }

    // stop ssl server
    m_state.m_server->stop();

    // stop thread pool
    if (m_config.m_multithreaded)
        m_thread_pool.stop();

}

void device_node::client_job(hc::net::ssl::server_conn_ptr conn_ptr, std::string data) {
    auto mit = m_conns.find(conn_ptr);
    if (mit == m_conns.end()) {
        hc::util::logger::dbg("connection has no associated handler, it was likely destroyed by another thread");
        return;
    }

    std::shared_ptr<protocol_handler> handler = mit->second;

    try {
        handler->execute(m_state, conn_ptr, data);
    } catch(hc::exception& e) {
        if (handler->get_destroyed()) {
            hc::util::logger::dbg("handler destroyed, likely cause of this exception");
            hc::util::logger::dbg("exception info: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");
        } else {
            hc::util::logger::err("exception occurred: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");
        }

        return;
    }
    
    if (!handler->get_destroyed() && handler->should_upgrade_protocol()) {
        m_state.m_server->toggle_timeout(conn_ptr);
        mit->second = handler->get_new_protocol();
    }
}