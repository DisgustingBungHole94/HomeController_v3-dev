#pragma once

#include "homecontroller/api/message.h"

namespace hc {
namespace api {
    
    class response : public message {
        public:
            response(bool should_check_success) 
                : m_should_check_success(should_check_success)
            {}

            ~response() {}

            void parse(const std::string& data) override;

            bool get_success() const { return m_success; }
            const std::string& get_error_msg() const { return m_error_msg; }
            int get_error_code() const { return m_error_code; }

        private:
            bool m_should_check_success;

            bool m_success;
            std::string m_error_msg;
            int m_error_code;
    };

}
}