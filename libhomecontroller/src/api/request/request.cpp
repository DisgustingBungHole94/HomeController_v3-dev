#include "homecontroller/api/request/request.h"

#include "homecontroller/exception.h"

namespace hc {
namespace api {

    void request::parse(const std::string& data) {
        json::document json_doc;

        json_doc.parse(data);
        get_data(json_doc);
    }

}
}