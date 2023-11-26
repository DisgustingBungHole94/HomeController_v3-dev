#pragma once

#include "state.h"
#include "protocol_handler/protocol_handler.h"
#include "protocol_handler/http_handler.h"

#include <homecontroller/net/ssl/server.h>
#include <homecontroller/thread/thread_pool.h>
#include <map>
#include <iostream>

class device_node : public std::enable_shared_from_this<device_node> {
    public:
        device_node() {}
        ~device_node() {}

        bool start();
        void shutdown();

    private:
        void on_connect(hc::net::ssl::server_conn_hdl conn_hdl);
        void on_data(hc::net::ssl::server_conn_hdl conn_hdl);
        void on_disconnect(hc::net::ssl::server_conn_hdl conn_hdl);

        void loop();

        void client_job(hc::net::ssl::server_conn_ptr conn_ptr, std::string data);

        state m_state;

        struct config {
            int m_port;
            std::string m_tls_cert_file;
            std::string m_tls_priv_key_file;

            int m_max_connections;
            int m_connection_expire_time;
            
            bool m_multithreaded;

            bool m_debug_mode;
        } m_config;

        hc::thread::thread_pool m_thread_pool;

        std::map< hc::net::ssl::server_conn_hdl, 
                            std::shared_ptr<protocol_handler>, 
                            std::owner_less<hc::net::ssl::server_conn_hdl>> m_conns;
};