#pragma once

#include <cstdint>
#include <iostream>

// 什么宏模仿,卧槽, 好强
#define container_of(ptr, type, member)                    \
    ({                                                     \
        const typeof(((type *)0)->member) *__mptr = (ptr); \
        (type *)((char *)__mptr - offsetof(type, member)); \
    })


namespace SimpleRedis {
// 简单hash一手,然后直接溢出,
inline uint64_t string_hash(const std::string& str) {
    uint32_t h = 0x811C9DC5;
    for (const auto c : str) {
        h = (h + (uint8_t)c) * 0x01000193;
    }
    return h;
}


}; // namespace SimpleRedis