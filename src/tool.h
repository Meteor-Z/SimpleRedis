#pragma once

#include <stddef.h>

// Node *my_node = some_lookup_function();
// MyData *my_data = container_of(my_node, MyData, node);
//                                 ^ptr     ^type   ^member
#define container_of(ptr, T, member)                                                     \
    ({                                                                                   \
        const typeof(((T*)0)->member)* __mptr = (ptr);                                   \
        (T*)((char*)__mptr - offsetof(T, member));                                       \
    })