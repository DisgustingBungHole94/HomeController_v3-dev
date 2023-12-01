#include "device_manager.h"

#include "protocol_handler/device_handler.h"
#include "protocol_handler/ws_handler.h"


#include <homecontroller/exception.h>
#include <homecontroller/util/logger.h>

std::shared_ptr<device> user::add_device(const std::string& device_id, std::shared_ptr<device_handler> device_handler_ptr) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::shared_ptr<device> device_ptr = std::make_shared<device>();
    device_ptr->set_id(device_id);
    device_ptr->set_device_handler(device_handler_ptr);

    m_devices.insert(std::make_pair(device_id, device_ptr));

    hc::util::logger::log("device [" + device_id + "] connected");

    return device_ptr;
}

void user::remove_device(const std::string& device_id) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto device_mit = m_devices.find(device_id);
    if (device_mit == m_devices.end()) {
        throw hc::exception("failed to disconnect device: device not found", "device_manager::disconnect_device");
    }

    m_devices.erase(device_mit);

    hc::util::logger::log("device [" + device_id + "] disconnected");
}

std::shared_ptr<device> user::get_device(const std::string& device_id) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto device_mit = m_devices.find(device_id);
    if (device_mit == m_devices.end()) {
        return nullptr;
    }

    return device_mit->second;
}

void user::add_associated_handler(std::shared_ptr<ws_handler> handler_ptr) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_associated_handlers.insert(handler_ptr);

    hc::util::logger::log("user [" + m_id + "] connected");
}

void user::remove_associated_handler(std::shared_ptr<ws_handler> handler_ptr) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_associated_handlers.erase(handler_ptr);
}

void device_manager::remove_user(const std::string& user_id, std::shared_ptr<ws_handler> ws_handler_ptr) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto user_mit = m_users.find(user_id);
    if (user_mit == m_users.end()) {
        throw hc::exception("failed to remove user: user not found", "device_manager::remove_user");
    }

    user_mit->second->remove_associated_handler(ws_handler_ptr);

    hc::util::logger::log("user [" + user_id + "] disconnected");
}

std::shared_ptr<user> device_manager::get_user(const std::string& user_id) {
    // devices call this function upon connect, so create user in case it doesn't exist
    
    std::lock_guard<std::mutex> lock(m_mutex);

    std::shared_ptr<user> user_ptr;
    
    auto user_mit = m_users.find(user_id);
    if (user_mit == m_users.end()) {
        user_ptr = std::make_shared<user>();
        user_ptr->set_id(user_id);

        m_users.insert(std::make_pair(user_id, user_ptr));
    } else {
        user_ptr = user_mit->second;
    }

    return user_ptr;
}