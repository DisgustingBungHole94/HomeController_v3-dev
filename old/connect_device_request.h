#pragma once

#include "homecontroller/api/request/request.h"

namespace hc {
namespace api {
    
    class connect_device_request : public request {
        public:
            connect_device_request()
                : request("POST", "/connect/device")
            {}

            ~connect_device_request() {}

            void set_device_id(const std::string& device_id) { m_device_id = device_id; }
            const std::string& get_device_id() const { return m_device_id; }

            std::string str() override;

        private:
            void get_data(json::document& json_doc) override;

            std::string m_device_id;
    };

}
}