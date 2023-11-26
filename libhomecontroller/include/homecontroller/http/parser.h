#pragma once

#include "homecontroller/http/request.h"
#include "homecontroller/http/response.h"

#include <llhttp.h>

namespace hc {
namespace http {
    
    class parser {
        public:

            enum class type {
                REQUEST, RESPONSE
            };

            parser() {}
            ~parser() {}

            void init(const type& type);
            bool parse(const std::string& data);

            request get_request();
            response get_response();

        private:
            llhttp_t m_parser;
            llhttp_settings_t m_settings;

            type m_type;

            struct data {
                // request
                std::string m_method;
                std::string m_url;

                // response
                std::string m_status;

                std::string m_body;

                std::string m_last_header;
                std::map<std::string, std::string> m_headers;

                bool m_upgrade = false;

                bool m_finished = false;
            } m_data;

    };

}
}