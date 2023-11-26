#pragma once

#include <string>

namespace hc {
namespace util {

    class logger {
        public:
            static void log(const std::string& msg, bool nl = true);
            static void err(const std::string& msg, bool nl = true);
            static void csh(const std::string& msg, bool nl = true);
            static void dbg(const std::string& msg, bool nl = true);

            static std::string timestamp();

            static void enable_debug() { _DEBUG_ENABLED = true; }
            static void disable_debug() { _DEBUG_ENABLED = false; }

        private:
            static const std::string _MASTER_PREFIX;

            static const std::string _LOG_PREFIX;
            static const std::string _ERR_PREFIX;
            static const std::string _CSH_PREFIX;
            static const std::string _DBG_PREFIX;

            static bool _DEBUG_ENABLED;

            static void print(const std::string& prefix, const std::string& msg, bool nl, const std::string& color = "");
    };

}
}