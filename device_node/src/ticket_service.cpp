#include "ticket_service.h"

#include <random>
#include <iostream>
#include <thread>

#include <homecontroller/util/bits.h>
#include <homecontroller/util/logger.h>
#include <homecontroller/api/info.h>

std::string ticket_service::create_user_ticket(const std::string& user_id) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string ticket = hc::util::bits::random_identifier(hc::api::info::TICKET_LENGTH);

    ticket_data data;
    data.m_type = type::USER;
    data.m_user_id = user_id;
    data.m_create_time = std::chrono::system_clock::now();
    data.m_used = false;

    m_tickets.insert(std::make_pair(ticket, data));

    return ticket;
}

std::string ticket_service::create_device_ticket(const std::string& user_id, const std::string& device_id) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string ticket = hc::util::bits::random_identifier(hc::api::info::TICKET_LENGTH);

    ticket_data data;
    data.m_type = type::DEVICE;
    data.m_user_id = user_id;
    data.m_device_id = device_id;
    data.m_create_time = std::chrono::system_clock::now();
    data.m_used = false;

    m_tickets.insert(std::make_pair(ticket, data));

    return ticket;
}

bool ticket_service::check_user_ticket(const std::string& ticket, std::string& user_id_ref) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto mit = m_tickets.find(ticket);
    if (mit == m_tickets.end()) {
        return false;
    }

    if (mit->second.m_type != type::USER || mit->second.m_used) {
        return false;
    }

    user_id_ref = mit->second.m_user_id;

    mit->second.m_used = true;

    return true;
}

bool ticket_service::check_device_ticket(const std::string& ticket, std::string& user_id_ref, std::string& device_id_ref) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto mit = m_tickets.find(ticket);
    if (mit == m_tickets.end()) {
        return false;
    }

    if (mit->second.m_type != type::DEVICE || mit->second.m_used) {
        return false;
    }

    user_id_ref = mit->second.m_user_id;
    device_id_ref = mit->second.m_device_id;

    mit->second.m_used = true;

    return true;
}

void ticket_service::ticket_expire_loop() {
    m_should_kill_ticket_expire_loop = false;

    while(!m_should_kill_ticket_expire_loop) {
        std::chrono::time_point<std::chrono::system_clock> curr_time = std::chrono::system_clock::now();

        for (auto mit = m_tickets.cbegin(); mit != m_tickets.cend();) {
            if (mit->second.m_used) {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_tickets.erase(mit++);
            }

            else if (std::chrono::duration_cast<std::chrono::milliseconds>(curr_time - mit->second.m_create_time).count() >= 5000) {
                std::lock_guard<std::mutex> lock(m_mutex);
                hc::util::logger::log("ticket [" + mit->first + "] expired");
                m_tickets.erase(mit++);
            } 
            
            else {
                ++mit;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

}