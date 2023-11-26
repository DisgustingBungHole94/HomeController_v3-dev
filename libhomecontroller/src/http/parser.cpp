#include "homecontroller/http/parser.h"

#include "homecontroller/util/str.h"
#include "homecontroller/util/logger.h"
#include "homecontroller/exception.h"

#include <iostream>

namespace hc {
namespace http {
    
    void parser::init(const type& type) {
        m_type = type;
        llhttp_type llhttp_type = (m_type == type::REQUEST) ? HTTP_REQUEST : HTTP_RESPONSE;

        llhttp_init(&m_parser, llhttp_type, &m_settings);
        m_parser.data = &m_data;

        llhttp_settings_init(&m_settings);

        m_settings.on_url = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
            data* data_ptr = static_cast<data*>(parser->data);
            data_ptr->m_url = std::string(at, len);

            return 0;
        };

        m_settings.on_status = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
            data* data_ptr = static_cast<data*>(parser->data);
            data_ptr->m_status = std::string(at, len);

            return 0;
        };

        m_settings.on_body = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
            data* data_ptr = static_cast<data*>(parser->data);
            data_ptr->m_body = std::string(at, len);

            return 0;
        };

        m_settings.on_header_field = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
            data* data_ptr = static_cast<data*>(parser->data);
            
            std::string header = std::string(at, len);
            data_ptr->m_last_header = hc::util::str::to_lower_case(header);

            return 0;
        };

        m_settings.on_header_value = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
            data* data_ptr = static_cast<data*>(parser->data);
            
            if (data_ptr->m_last_header == "") {
                return -1;
            }

            std::string value(at, len);

            data_ptr->m_headers.insert(std::make_pair(data_ptr->m_last_header, value));
            data_ptr->m_last_header = "";

            return 0;
        };

        m_settings.on_message_complete = [](llhttp_t* parser) -> int {
            data* data_ptr = static_cast<data*>(parser->data);
            data_ptr->m_finished = true;

            return 0;
        };

    }

    bool parser::parse(const std::string& data) {
        llhttp_errno err = llhttp_execute(&m_parser, data.c_str(), data.size());

        if (err == HPE_PAUSED_UPGRADE) {
            m_data.m_upgrade = true;
        } else if (err != HPE_OK) {
            util::logger::dbg("http parser error: " + std::string(llhttp_errno_name(err)));
            throw exception("failed to parse HTTP request", "hc::http::parser::parse");
        }

        if (!m_data.m_finished) {
            return false;
        }

        if (m_type == type::REQUEST) {
            m_data.m_method = std::string(llhttp_method_name(static_cast<llhttp_method_t>(m_parser.method)));
        }

        return true;
    }

    request parser::get_request() {
        if (m_type != type::REQUEST) {
            throw exception("wrong parser type", "hc::http::parser::get_request");
        }

        request request(m_data.m_method, m_data.m_url, m_data.m_body, m_data.m_headers);
        request.set_should_upgrade(m_data.m_upgrade);

        m_data = data();

        return request;
    }

    response parser::get_response() {
        if (m_type != type::RESPONSE) {
            throw exception("wrong parser type", "hc::http::parser::get_request");
        }

        response response(m_data.m_status, m_data.m_body, m_data.m_headers);
        response.set_should_upgrade(m_data.m_upgrade);

        m_data = data();

        return response;
    }

}
}