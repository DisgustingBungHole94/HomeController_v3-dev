#pragma once

#include "device_manager.h"

#include <homecontroller/net/ssl/server.h>
#include <homecontroller/api/request_maker.h>

#include <memory>

struct state {
    enum class operation {
        RUNNING, STOPPED
    } m_operation;

    std::string m_secret;

    std::shared_ptr<hc::net::ssl::server> m_server;
    std::shared_ptr<hc::api::request_maker> m_api_request_maker;

    std::shared_ptr<device_manager> m_device_manager;
};
