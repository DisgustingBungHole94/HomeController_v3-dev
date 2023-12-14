#include "protocol_handler.h"

#include <homecontroller/util/logger.h>

#include <iostream>

void protocol_handler::execute(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr) {
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_destroyed) {
        return;
    }

    on_data(state, conn_ptr);
}

void protocol_handler::on_destroyed(const state& state) {}

void protocol_handler::on_data(const state& state, const hc::net::ssl::server_conn_ptr& conn_ptr) {}

void protocol_handler::start_checking_connection() {
    hc::util::logger::log("starting check connection loop for client handler");

    m_check_connection_thread = std::thread([this]() -> void {
        while(this->m_should_check_connection) {
            check_connection();
        }
    });
}

void protocol_handler::stop_checking_connection() {
    hc::util::logger::log("stopping check connection loop for client handler");

    m_should_check_connection = false;
    m_check_connection_thread.join();
}


bool protocol_handler::check_connection() {
    return false;
}