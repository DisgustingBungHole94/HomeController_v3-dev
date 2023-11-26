#pragma once

#include <string>
#include <vector>

namespace hc {
namespace util {
namespace str {
    
    extern std::string to_lower_case(std::string str);
    extern std::vector<std::string> split_string(std::string str, char delim);

}
}
}