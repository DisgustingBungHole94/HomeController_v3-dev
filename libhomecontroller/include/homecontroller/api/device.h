#pragma once

#include "homecontroller/net/ssl/client_connection.h"
#include "homecontroller/api/state.h"

#include <functional>
#include <string>

namespace hc {
namespace api {
    
    class device {
        public:
            device() 
                : device(nullptr, "", "")
            {}

            device(net::ssl::client_conn_ptr conn_ptr, const std::string& device_id, const std::string& ticket) 
                : m_conn_ptr(conn_ptr), m_device_id(device_id), m_ticket(ticket), m_running(false)
            {}

            ~device() {}

            void run(state::power power, const std::string& state_data);
            void stop();

            void set_state(const state& state);
            const state& get_state() { return m_state; }

            void set_turn_on_callback(std::function<void()> callback) { m_turn_on_callback = callback; }
            void set_turn_off_callback(std::function<void()> callback) { m_turn_off_callback = callback; }
            void set_data_callback(std::function<std::string(std::string)> callback) { m_data_callback = callback; }
            void set_get_state_callback(std::function<std::string()> callback) { m_get_state_callback = callback; }

            bool is_connected() { return (m_conn_ptr == nullptr) || !m_conn_ptr->is_closed(); }

        private:
            void send_notify_packet();

            net::ssl::client_conn_ptr m_conn_ptr;

            std::function<void()> m_turn_on_callback;
            std::function<void()> m_turn_off_callback;
            std::function<std::string(std::string)> m_data_callback;
            std::function<std::string()> m_get_state_callback;

            std::string m_device_id;
            std::string m_ticket;

            bool m_running;

            state m_state;
    };

}
}