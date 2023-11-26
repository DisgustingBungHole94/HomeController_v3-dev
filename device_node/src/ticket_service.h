#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <chrono>

class ticket_service {
    public:
        enum class type {
            DEVICE, USER
        };

        ticket_service() {}
        ~ticket_service() {}

        std::string create_user_ticket(const std::string& user_id);
        std::string create_device_ticket(const std::string& user_id, const std::string& device_id);

        bool check_user_ticket(const std::string& ticket, std::string& user_id_ref);
        bool check_device_ticket(const std::string& ticket, std::string& user_id_ref, std::string& device_id_ref);

        void ticket_expire_loop();
        void kill_ticket_expire_loop() { m_should_kill_ticket_expire_loop = true; }

    private:
        struct ticket_data {
            type m_type;
            std::string m_user_id;
            std::string m_device_id; // unused for user
            std::chrono::time_point<std::chrono::system_clock> m_create_time;
            bool m_used;
        };

        std::unordered_map<std::string, ticket_data> m_tickets;

        std::mutex m_mutex;
        bool m_should_kill_ticket_expire_loop;
};