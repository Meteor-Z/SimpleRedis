#include <cstdint>
#include <iostream>
#include <gtest/gtest.h>
#include "../../src/avl.h"
#include "../../src/zset.h"

TEST(zset_verify_small, zset) {
    SimpleRedis::Container containner {};
    for (uint32_t i = 0; i < 100; i++) {
        containner.add(i);
    }

    SimpleRedis::AVLNode* min = containner.m_root;
    while (min->m_left) {
        min = min->m_left;
    }

    for (uint32_t i = 0; i < 100; i++) {
        SimpleRedis::AVLNode* node = SimpleRedis::avl_offset(min, (int64_t)i);
        assert(container_of(node, Data, m_node)->val == i);
        for (uint32_t j = 0; j < 100; j++) {
            int64_t offset = (int64_t)j - (int64_t)i;
            SimpleRedis::AVLNode* n2 = SimpleRedis::avl_offset(node, offset);
            assert(container_of(n2, SimpleRedis::Data, m_node)->val == j);
        }
        assert(!avl_offset(node, -(int64_t)i - 1));
        assert(!avl_offset(node, sz - i));
    }

    SimpleRedis::dispose(containner);
}
int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}