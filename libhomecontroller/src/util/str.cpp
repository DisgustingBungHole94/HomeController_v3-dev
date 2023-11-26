#include "homecontroller/util/str.h"

#include <algorithm>

namespace hc {
namespace util {
namespace str {
    
    std::string to_lower_case(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) -> unsigned char {
            return std::tolower(c);
        });

        return str;
    }

    std::vector<std::string> split_string(std::string str, char delim) {
        std::size_t pos = 0;
        std::vector<std::string> tokens;

        while((pos = str.find(delim)) != std::string::npos) {
            tokens.push_back(str.substr(0, pos));
            str.erase(0, pos + 1);
        }

        tokens.push_back(str);

        return tokens;
    }

}
}
}