#include <homecontroller/api/request_maker.h>
#include <homecontroller/exception.h>
#include <homecontroller/util/logger.h>

int main() {
    hc::api::request_maker api;
    hc::api::device device;

    hc::util::logger::enable_debug();

    try {
        api.connect();

        device = api.login_device("test", "1234", "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB");
        device.run(false);

    } catch(hc::exception& e) {
        hc::util::logger::err(std::string(e.what()) + " (" + std::string(e.func()) + ")");
        device.stop();
    }

    if (api.is_connected()) api.disconnect();
    if (device.is_connected()) device.stop();

    return 0;
}