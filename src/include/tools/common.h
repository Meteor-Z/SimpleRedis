#pragma once

#include <string>
#include <cstdint>
#include <fcntl.h>

namespace SimpleRedis {
/**
 * @brief
 * 因为这个存储的是当前当前数据，所以要得到他真正的数值，因为还有指针
 *
 */
#define container_of(ptr, type, member)                                                  \
    ({                                                                                   \
        const typeof(((type*)0)->member)* __mptr = (ptr);                                \
        (type*)((char*)__mptr - offsetof(type, member));                                 \
    })

// 简单hash一手,然后直接溢出,
inline uint64_t string_hash(const std::string& str) {
    uint32_t h = 0x811C9DC5;
    for (const auto c : str) {
        h = (h + (uint8_t)c) * 0x01000193;
    }
    return h;
}

// 设置非堵塞
inline void fd_set_not_block(int fd) {
    int is_block = fcntl(fd, F_GETFL, 0);
    if (is_block & O_NONBLOCK) {
        return;
    }
    // 对这个文件描述符进行设置，设置成非堵塞,
    fcntl(fd, F_SETFL, is_block | O_NONBLOCK);

}

}; // namespace SimpleRedis