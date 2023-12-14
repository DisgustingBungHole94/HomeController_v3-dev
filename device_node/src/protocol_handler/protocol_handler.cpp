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