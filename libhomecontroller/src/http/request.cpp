#include "homecontroller/http/request.h"

namespace hc {
namespace http {

    std::string request::str() const {
        std::string request = m_method + " " + m_url + " HTTP/1.1\r\n";

        for (auto& x : m_headers) {
            request += x.first + ": " + x.second;
            request += "\r\n";
        }

        request += "\r\n" + m_body;
        return request;
    }

}
}