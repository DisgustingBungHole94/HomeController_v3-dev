#pragma once

#include "homecontroller/http/response.h"

#include <string>

namespace hc {
namespace http {
namespace common {
    
    inline response error(const std::string& method, const std::string& msg, int code) {
        std::string json_str = 
            "{"
                "\"success\":false,"
                "\"errorMsg\":\"" + msg + "\","
                "\"errorCode\":" + std::to_string(code) + ""
            "}";
        
        response res = response(method, json_str);
        res.add_header("Content-Type", "application/json");
        return res;
    }

    inline response bad_request() {
        return error("400 Bad Request", "bad http request", 100);
    }

    inline response need_auth() {
        return error("401 Unauthorized", "not authorized", 200);
    }

    inline response not_found() {
        return error("404 Not Found", "resource not found", 300);
    }

    inline response server_error() {
        return error("500 Internal Server Error", "internal server error", 400);
    }

    inline response method_not_allowed() {
        return error("405 Method Not Allowed", "method not allowed", 500);
    }

}
}
}