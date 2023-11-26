#pragma once

#include "homecontroller/json/document.h"

namespace hc {
namespace api {

    class message {
        public:
            message() {}
            ~message() {}

            virtual void parse(const std::string& data);

            virtual std::string str();

        protected:
            virtual void get_data(json::document& json_doc);

            

    };

}
}