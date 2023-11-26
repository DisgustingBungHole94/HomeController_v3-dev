#pragma once

#include "homecontroller/api/request/request.h"

namespace hc {
namespace api {
    
    class register_device_request : public request {
        public:
            register_device_request()
                : request("POST", "/register_device")
            {}

            ~register_device_request() {}

            void set_device_type(const std::string& device_type) { m_device_type = device_type; }
            const std::string& get_device_type() const { return m_device_type; }

            void set_device_name(const std::string& device_name) { m_device_name = device_name; }
            const std::string& get_device_name() const { return m_device_name; }

            void set_device_note(const std::string& device_note) { m_device_note = device_note; }
            const std::string& get_device_note() const { return m_device_note; }

            std::string str() override;

        private:
            void get_data(json::document& json_doc) override;

            std::string m_device_type;
            std::string m_device_name;
            std::string m_device_note;
    };

}
}