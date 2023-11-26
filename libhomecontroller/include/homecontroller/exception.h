#pragma once

#include <exception>
#include <string>

namespace hc {
    class exception : public std::exception {
        public:
            exception()
                : exception("hc::exception", "::")
            {}

            exception(const std::string& what, const std::string& func)
                : m_what(what), m_func(func)
            {}

            const char* what() const throw() {
                return m_what.c_str();
            }

            const char* func() const throw() {
                return m_func.c_str();
            }

        private:
            std::string m_what;
            std::string m_func;
    };
}