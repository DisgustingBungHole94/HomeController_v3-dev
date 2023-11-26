#include "homecontroller/api/response/response.h"

#include "homecontroller/exception.h"

#include <iostream>

namespace hc {
namespace api {

    void response::parse(const std::string& data) {
        json::document json_doc;
        json_doc.parse(data);

        if (m_should_check_success) {
            if (!json_doc.get_bool("success")) {
                m_success = false;
                m_error_msg = json_doc.get_string("errorMsg");
                m_error_code = json_doc.get_int("errorCode");

                return;
            } else {
                m_success = true;
            }
        }

        get_data(json_doc);
    }

}
}