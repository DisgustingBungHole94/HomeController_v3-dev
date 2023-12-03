#pragma once

#include <string>

namespace hc {
namespace api {
    
    class device_state {
        public:
            device_state() {}
            ~device_state() {}

            virtual bool parse(const std::string& data);
            virtual std::string serialize() const;
    };

}
}