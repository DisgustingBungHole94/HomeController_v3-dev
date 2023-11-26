#include "homecontroller/json/document.h"

#include "homecontroller/exception.h"

#include <rapidjson/error/en.h>

namespace hc {
namespace json {

    void document::parse(const std::string& data) {
        rapidjson::ParseResult res = m_rj_document.Parse(data.c_str());

        if (!res) {
            throw exception("failed to parse json: " + std::string(rapidjson::GetParseError_En(res.Code())), "hc::json::document::parse");
        }
    }

    std::string document::get_string(const std::string& name) {
        return std::any_cast<std::string>(get_value(name, value_type::STRING));
    }

    int document::get_int(const std::string& name) {
        return std::any_cast<int>(get_value(name, value_type::INT));
    }

    double document::get_double(const std::string& name) {
        return std::any_cast<double>(get_value(name, value_type::DOUBLE));
    }

    bool document::get_bool(const std::string& name) {
        return std::any_cast<bool>(get_value(name, value_type::BOOL));
    }

    void document::get_object(const std::string& name, document& document_ref) {
        rapidjson::Value::MemberIterator itr = m_rj_document.FindMember(name.c_str());

        if (itr == m_rj_document.MemberEnd()) {
            throw exception("member \"" + name + "\" not found", "hc::json::document::get_object");
        } else if (!itr->value.IsObject()) {
            throw exception("member \"" + name + "\" is not expected type of object", "hc::json::document::get_object");
        }

        document_ref.m_rj_document.Swap(itr->value);
    }

    std::any document::get_value(const std::string& name, const value_type& type) {
        if (!m_rj_document.IsObject()) {
            throw exception("bad document", "hc::json::document::get_value");
        }

        rapidjson::Value::ConstMemberIterator itr = m_rj_document.FindMember(name.c_str());

        if (itr == m_rj_document.MemberEnd()) {
            throw exception("member \"" + name + "\" not found", "hc::json::document::get_value");
        }

        std::any value;

        std::string type_name;
        bool wrong_type = false;

        switch(type) {
            case value_type::STRING:
                type_name = "string";
                if (!itr->value.IsString()) {
                    wrong_type = true;
                    break;
                }
                value = std::string(itr->value.GetString());
                break;
            case value_type::INT:
                type_name = "int";
                if (!itr->value.IsInt()) {
                    wrong_type = true;
                    break;
                }
                value = itr->value.GetInt();
                break;
            case value_type::DOUBLE:
                type_name = "double";
                if (!itr->value.IsDouble()) {
                    wrong_type = true;
                    break;
                }
                value = itr->value.GetDouble();
                break;
            case value_type::BOOL:
                type_name = "bool";
                if (!itr->value.IsBool()) {
                    wrong_type = true;
                    break;
                }
                value = itr->value.GetBool();
                break;
        }

        if (wrong_type) {
            throw exception("member \"" + name + "\" is not expected type of " + type_name, "hc::json::document::get_value");
        }

        return value;
    }

}
}