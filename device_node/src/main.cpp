#include "device_node.h"

#include <homecontroller/util/logger.h>

#include <csignal>

device_node app;

int main() {
    std::signal(SIGINT, [](int s) {
        app.shutdown();
    });

    std::signal(SIGPIPE, [](int s) {
        hc::util::logger::dbg("WARNING: received SIGPIPE");
    });

    if (!app.start()) {
        hc::util::logger::csh("exiting with non-zero status code");
        return -1;
    }

    hc::util::logger::log("exiting normally");
    return 0;
}