#pragma once

#include <string>
#include <unordered_map>
#include <set>
#include <homecontroller/net/ssl/server_connection.h>
#include <homecontroller/api/state.h>

class device_handler;
class ws_handler;

class device;

class user {
    public:
        user() {}
        ~user() {}

        void set_id(const std::string& id) { m_id = id; }
        const std::string& get_id() { return m_id; }

        std::shared_ptr<device> add_device(const std::string& device_id, std::shared_ptr<device_handler> device_handler_ptr);
        void remove_device(const std::string& device_id);

        std::shared_ptr<device> get_device(const std::string& device_id);
        const std::unordered_map<std::string, std::shared_ptr<device>>& get_devices() { return m_devices; }

        void add_associated_handler(std::shared_ptr<ws_handler> handler_ptr);
        void remove_associated_handler(std::shared_ptr<ws_handler> handler_ptr);

        const std::set<std::shared_ptr<ws_handler>, std::owner_less<std::shared_ptr<ws_handler>>>& get_associated_handlers() { return m_associated_handlers; }

    private:
        std::string m_id;

        std::unordered_map<std::string, std::shared_ptr<device>> m_devices;
        std::set<std::shared_ptr<ws_handler>, std::owner_less<std::shared_ptr<ws_handler>>> m_associated_handlers;

        std::mutex m_mutex;
};

class device {
    public:
        device() {}
        ~device() {}

        void set_id(const std::string& id) { m_id = id; }
        const std::string& get_id() { return m_id; }

        void set_state(const hc::api::state& state) { m_state = state; }
        const hc::api::state& get_state() { return m_state; }

        void set_device_handler(std::shared_ptr<device_handler> device_handler_ptr) { m_device_handler_ptr = device_handler_ptr; }
        std::shared_ptr<device_handler> get_device_handler() { return m_device_handler_ptr; }

    private:
        std::string m_id;

        hc::api::state m_state;

        std::shared_ptr<device_handler> m_device_handler_ptr;
};

class device_manager {
    public:
        device_manager() {}
        ~device_manager() {}

        void remove_user(const std::string& user_id, std::shared_ptr<ws_handler> ws_handler_ptr);

        std::shared_ptr<user> get_user(const std::string& user_id);

    private:
        std::unordered_map<std::string, std::shared_ptr<user>> m_users;

        std::mutex m_mutex;

};