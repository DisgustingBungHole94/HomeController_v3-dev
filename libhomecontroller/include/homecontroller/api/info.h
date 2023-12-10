#pragma once

#include <string>

namespace hc {
namespace api {
namespace info {

    static const std::string API_HOST =                 "bunghole.pro";
    static const std::string API_PORT =                 "4435";

    static const std::string AUTH_SCHEMA =              "HCAuth";
    static const std::string DEVICE_UPGRADE_SCHEMA =    "HCDevice";

    static const std::string NAME =                     "HomeController";
    static const std::string VERSION =                  "0.0.4";

    static const uint16_t MAGIC =                       0xBEEF;

#ifdef _WIN32
    static const std::string OS =                       "Windows";
#endif
#if defined(unix) || defined(__unix__) || defined(__unix)
    static const std::string OS =                       "Linux";
#endif
#ifdef __APPLE__
    static const std::string OS =                       "macOS";
#endif

    static const std::size_t DEVICE_ID_LENGTH =         32;
    static const std::size_t TICKET_LENGTH =            32;

}
}
}