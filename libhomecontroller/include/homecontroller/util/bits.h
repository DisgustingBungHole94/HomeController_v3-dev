#pragma once

#include <cstdint>
#include <string>

namespace hc {
namespace util {
namespace bits {
    
    extern uint16_t bytes_to_u16(const uint8_t* b);
    extern uint32_t bytes_to_u32(const uint8_t* b);
    extern uint64_t bytes_to_u64(const uint8_t* b);

    extern uint64_t bytes_to_uint(const uint8_t* b, int width);

    extern std::string u8_to_str(uint8_t val);
    extern std::string u16_to_str(uint16_t val);
    extern std::string u32_to_str(uint32_t val);
    extern std::string u64_to_str(uint64_t val);

    extern std::string uint_to_str(uint64_t val, int width);

    extern std::string random_identifier(std::size_t length);

}
}
}