#pragma once

#include <cstdint>

namespace SimpleRedis {
struct AVLNode {

    uint32_t m_depth { 0 };
    uint32_t m_cnt { 0 };
    AVLNode* m_left { nullptr };
    AVLNode* m_right { nullptr };
    AVLNode* m_parent { nullptr };
};

void avl_init(AVLNode* node);
uint32_t avl_depth(AVLNode* node);

uint32_t avl_cnt(AVLNode* node);

void avl_update(AVLNode* node);

// 左旋
AVLNode* rot_left(AVLNode* node);

// 右旋
AVLNode* rot_right(AVLNode* node);

AVLNode* avl_fix_left(AVLNode* root);

AVLNode* avl_fix_right(AVLNode* root);

// 根据高度进行左旋和右旋修复
AVLNode* avl_fix(AVLNode* node);

AVLNode* avl_del(AVLNode* node);



struct Data {
public:
    AVLNode m_node {};
    uint32_t val { 0 };
};

class Container {
public:
    AVLNode* m_root { nullptr };
    void add(uint32_t val);
    bool del(uint32_t val);
};

void dispose(Container& container);


}; // namespace SimpleRedis