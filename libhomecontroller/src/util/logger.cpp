#include "homecontroller/util/logger.h"

#include <ctime>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace hc {
namespace util {

    const std::string logger::_MASTER_PREFIX = "[hc]";

    const std::string logger::_LOG_PREFIX = "[LOG]   ";
    const std::string logger::_ERR_PREFIX = "[ERR]   ";
    const std::string logger::_CSH_PREFIX = "[FATAL] ";
    const std::string logger::_DBG_PREFIX = "[DEBUG] ";

    bool logger::_DEBUG_ENABLED = false;

    void logger::log(const std::string& msg, bool nl) {
        print(_LOG_PREFIX, msg, nl);
    }

    void logger::err(const std::string& msg, bool nl) {
        print(_ERR_PREFIX, msg, nl, "\033[0;93m");
    }

    void logger::csh(const std::string& msg, bool nl) {
        print(_CSH_PREFIX, msg, nl, "\033[0;91m");
    }

    void logger::dbg(const std::string& msg, bool nl) {
        if (_DEBUG_ENABLED) print(_DBG_PREFIX, msg, nl, "\033[3;90m");
    }

    std::string logger::timestamp() {
        auto t = std::time(nullptr);

        std::ostringstream ss;
        ss << std::put_time(std::localtime(&t), "%m-%d-%y %H:%M:%S");

        return ss.str();
    }

    void logger::print(const std::string& prefix, const std::string& msg, bool nl, const std::string& color) {
        std::ostringstream ss;
        ss << color << _MASTER_PREFIX << " " << timestamp() << " " << prefix << "\t" << msg;
        if (nl) ss << "\n";
        ss << "\033[0m"; // default color

        std::cout << ss.str();
     }

}
}