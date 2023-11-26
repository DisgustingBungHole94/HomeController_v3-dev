#pragma once

#include <rapidjson/document.h>

#include <string>
#include <any>

namespace hc {
namespace json {

    class document {
        public:
            document() {}
            ~document() {}

            void parse(const std::string& data);

            std::string get_string(const std::string& name);
            int get_int(const std::string& name);
            double get_double(const std::string& name);
            bool get_bool(const std::string& name);
            void get_object(const std::string& name, document& document_ref);

            const std::string& get_error() { return m_error; }

        private:
            enum class value_type {
                STRING, INT, DOUBLE, BOOL
            };

            std::any get_value(const std::string& name, const value_type& type);

            rapidjson::Document m_rj_document;

            std::string m_error;
    };

}
}