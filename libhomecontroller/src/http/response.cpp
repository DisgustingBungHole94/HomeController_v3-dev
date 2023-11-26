#include "homecontroller/http/response.h"

namespace hc {
namespace http {

    std::string response::str() const {
        std::string response = "HTTP/1.1 " + m_status + "\r\n";

        for (auto& x : m_headers) {
            response += x.first + ": " + x.second;
            response += "\r\n";
        }

        response += "\r\n" + m_body;
        return response;
    }

}
}