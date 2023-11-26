#include "homecontroller/util/bits.h"

#include <limits.h>
#include <random>

namespace hc {
namespace util {
namespace bits {
    
    uint16_t bytes_to_u16(const uint8_t* b) {
        return static_cast<uint16_t>(bytes_to_uint(b, 16));
    }

    uint32_t bytes_to_u32(const uint8_t* b) {
        return static_cast<uint32_t>(bytes_to_uint(b, 32));
    }

    uint64_t bytes_to_u64(const uint8_t* b) {
        return static_cast<uint64_t>(bytes_to_uint(b, 64));
    }

    uint64_t bytes_to_uint(const uint8_t* b, int width) {
        constexpr int NUM_BITS_IN_BYTE = CHAR_BIT;

        uint64_t num = 0;
        for (int i = 0; i < width / NUM_BITS_IN_BYTE; i++) {
            num |= (static_cast<uint64_t>(b[i]) << NUM_BITS_IN_BYTE * i);
        }

        return num;
    }

    std::string u8_to_str(uint8_t val) {
        return uint_to_str(val, 8);
    }

    std::string u16_to_str(uint16_t val) {
        return uint_to_str(val, 16);
    }

    std::string u32_to_str(uint32_t val) {
        return uint_to_str(val, 32);
    }

    std::string u64_to_str(uint64_t val) {
        return uint_to_str(val, 64);
    }

    std::string uint_to_str(uint64_t val, int width) {
        constexpr int NUM_BITS_IN_BYTE = CHAR_BIT;

        std::string str;
        str.resize(width / NUM_BITS_IN_BYTE);

        for (int i = 0; i < width / NUM_BITS_IN_BYTE; i++) {
            str[i] = (val >> i * NUM_BITS_IN_BYTE) & 0xFF;
        }

        return str;
    }

    std::string random_identifier(std::size_t length) {
        static const char CHAR_SET[] = 
        {   
            '0','1','2','3','4',
            '5','6','7','8','9',
            'A','B','C','D','E','F',
            'G','H','I','J','K',
            'L','M','N','O','P',
            'Q','R','S','T','U',
            'V','W','X','Y','Z',
            'a','b','c','d','e','f',
            'g','h','i','j','k',
            'l','m','n','o','p',
            'q','r','s','t','u',
            'v','w','x','y','z'
        };

        std::default_random_engine eng(std::random_device{}());
        std::uniform_int_distribution<int> dist(0, sizeof(CHAR_SET) - 1);

        std::string id;
        id.reserve(length);

        for (int i = 0; i < length; i++) {
            id += CHAR_SET[dist(eng)];
        }

        return id;
    }

}
}
}