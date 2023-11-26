#include "homecontroller/api/message.h"

#include "homecontroller/exception.h"

namespace hc {
namespace api {

    void message::parse(const std::string& data) {}

    std::string message::str() {
        return "";
    }

    void message::get_data(json::document& json_doc) {}

}
}