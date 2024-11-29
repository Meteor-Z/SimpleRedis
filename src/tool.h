#pragma once

#include <cstdint>
#include <stddef.h>

// Node *my_node = some_lookup_function();
// MyData *my_data = container_of(my_node, MyData, node);
//                                 ^ptr     ^type   ^member
#define container_of(ptr, T, member)                                                     \
    ({                                                                                   \
        const typeof(((T*)0)->member)* __mptr = (ptr);                                   \
        (T*)((char*)__mptr - offsetof(T, member));                                       \
    })


inline uint64_t str_hash(const uint8_t* data, size_t len) {
    uint32_t hash = 0x811C9DC5;
    for (size_t i = 0; i < len; i++) {
        hash = (hash + data[i]) % 0x01000193;
    }
    return hash;
}