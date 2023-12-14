#pragma once

#include "../state.h"

#include <homecontroller/net/ssl/server_connection.h>

#include <mutex>
#include <thread>

class protocol_handler {
    public:
        protocol_handler() 
            : m_should_upgrade_protocol(false), m_destroyed(false)
        {}

        ~protocol_handler() {}

        virtual void execute(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr);

        virtual void on_destroyed(const state& state);

        void start_checking_connection();
        void stop_checking_connection();

        void set_destroyed(bool destroyed) { m_destroyed = destroyed; }
        bool get_destroyed() { return m_destroyed; }

        bool should_upgrade_protocol() { return m_should_upgrade_protocol; }
        std::shared_ptr<protocol_handler> get_new_protocol() { return m_new_protocol; }

    protected:
        virtual void on_data(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr);

        virtual bool check_connection();

        bool m_should_check_connection;
        std::thread m_check_connection_thread;

        bool m_destroyed;

        bool m_should_upgrade_protocol;
        std::shared_ptr<protocol_handler> m_new_protocol;

        std::mutex m_mutex;

};