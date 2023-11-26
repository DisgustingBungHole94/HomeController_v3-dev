#include "homecontroller/http/generic.h"

namespace hc {
namespace http {

    void generic::add_header(const std::string& name, const std::string& value) {
        m_headers.insert(std::make_pair(name, value));
    }

    bool generic::get_header(const std::string& name, std::string& value_ref) const {
        auto mit = m_headers.find(name);
        if (mit == m_headers.end()) {
            return false;
        }

        value_ref = mit->second;
        return true;
    }

    std::string generic::str() const {
        return "";
    }

}
}