#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include "../../src/tool.h"
#include "../../src/avl.h"
#include <gtest/gtest.h>
#include <set>

static void avl_verify(SimpleRedis::AVLNode* parent, SimpleRedis::AVLNode* node) {
    if (!node) {
        return;
    }

    avl_verify(node, node->m_left);
    avl_verify(node, node->m_right);

    assert(node->m_parent == parent);

    assert(node->cnt == 1 + avl_cnt(node->m_left) + avl_cnt(node->m_right));

    uint32_t l = avl_depth(node->m_left);
    uint32_t r = avl_depth(node->m_right);
    assert(node->depth == 1 + std::max(l, r));

    assert(l == r || l + 1 == r || l == r + 1);

    uint32_t val = container_of(node, SimpleRedis::Data, m_node)->val;

    if (node->m_left) {
        assert(node->m_left->m_parent == node);
        assert(container_of(node->m_left, Data, node)->val <= val);
    }
    if (node->m_right) {
        assert(node->m_right->m_parent == node);
        assert(container_of(node->m_right, Data, node)->val >= val);
    }
}

// 其实就是一个中序遍历，然后塞入这个没有排序的extracted里面，然后看数据是否正常
static void extract(SimpleRedis::AVLNode* node, std::multiset<uint32_t>& extracted) {
    if (!node) {
        return;
    }
    extract(node->m_left, extracted);
    // containner_of就是那个按照偏移量来的到真正的数据的
    extracted.insert(container_of(node, SimpleRedis::Data, m_node)->val);
    extract(node->m_right, extracted);
}

static void check_avl(SimpleRedis::Container& containner,
                      const std::multiset<uint32_t>& ref) {
    avl_verify(nullptr, containner.m_root);
    // 验证数据是否正常
    assert(avl_cnt(containner.m_root) == ref.size());
    std::multiset<uint32_t> tmp {};
    extract(containner.m_root, tmp);
    // 判断数据是否正常
    assert(tmp == ref);
}

// 对数据结构进行测试
TEST(avl_verify_small, avl) {
    SimpleRedis::Container container {};
    std::multiset<uint32_t> ref {};
    for (uint32_t i = 0; i < 100; i++) {
        uint32_t val = (uint32_t)rand() % 1000;
        container.add(val);
        ref.insert(val);
        check_avl(container, ref);
    }

}

int main(int argc, char* argv[]) {
    // SimpleRedis::Container container {};
    // std::multiset<uint32_t> ref {};
    // for (uint32_t i = 0; i < 100; i++) {
    //     uint32_t val = (uint32_t)rand() % 1000;
    //     std::cout << val << std::endl;
    //     container.add(val);
    //     ref.insert(val);
    //     check_avl(container, ref);
    // }

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}