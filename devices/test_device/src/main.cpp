#include <homecontroller/api/request_maker.h>
#include <homecontroller/exception.h>
#include <homecontroller/util/logger.h>

void turn_on_callback() {
    hc::util::logger::log("Turning on!");
}

void turn_off_callback() {
    hc::util::loggser::log("Turning off!");
}

std::string data_callback(std::string data) {
    hc::util::logger::log("Received data: " + data);
    return "Hello, world!";
}

int main() {
    hc::api::request_maker api;
    hc::api::device device;

    hc::util::logger::enable_debug();

    try {
        api.connect();

        device = api.login_device("test", "1234", "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB");

        device.set_turn_on_callback(turn_on_callback);
        device.set_turn_off_callback(turn_off_callback);
        device.set_data_callback(data_callback);

        device.run(hc::api::state::power::OFF, "Hello, world!");

    } catch(hc::exception& e) {
        hc::util::logger::err(std::string(e.what()) + " (" + std::string(e.func()) + ")");
        device.stop();
    }

    if (api.is_connected()) api.disconnect();
    if (device.is_connected()) device.stop();

    return 0;
}