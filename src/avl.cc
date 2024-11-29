#include "avl.h"
#include "tool.h"
#include <algorithm>
#include <cassert>
#include <cstdint>

namespace SimpleRedis {

void avl_init(AVLNode* node) {
    node->m_depth = 1;
    node->m_cnt = 1;
    node->m_left = node->m_right = node->m_parent = nullptr;
}

uint32_t avl_depth(AVLNode* node) { return node ? node->m_depth : 0; }

uint32_t avl_cnt(AVLNode* node) { return node ? node->m_cnt : 0; }

void avl_update(AVLNode* node) {
    node->m_depth = std::max(avl_depth(node->m_left), avl_depth(node->m_right)) + 1;
    node->m_cnt = avl_cnt(node->m_left) + avl_cnt(node->m_right) + 1;
}

AVLNode* rot_left(AVLNode* node) {
    AVLNode* new_node = node->m_right;
    if (new_node->m_left) {
        new_node->m_left->m_parent = node;
    }

    node->m_right = new_node->m_left; // rotation
    new_node->m_left = node;          // rotation
    new_node->m_parent = node->m_parent;
    node->m_parent = new_node;
    avl_update(node);
    avl_update(new_node);
    return new_node;
}

AVLNode* rot_right(AVLNode* node) {

    AVLNode* new_node = node->m_left;
    if (new_node->m_right) {
        new_node->m_right->m_parent = node;
    }
    node->m_left = new_node->m_right;
    new_node->m_right = node;
    new_node->m_parent = node->m_parent;
    node->m_parent = new_node;
    avl_update(node);
    avl_update(new_node);
    return new_node;
}

AVLNode* avl_fix_left(AVLNode* root) {
    if (avl_depth(root->m_left->m_left) < avl_depth(root->m_left->m_right)) {
        root->m_left = rot_left(root->m_left);
    }
    return rot_right(root);
}

AVLNode* avl_fix_right(AVLNode* root) {
    if (avl_depth(root->m_right->m_left) > avl_depth(root->m_right->m_right)) {
        root->m_right = rot_right(root->m_right);
    }
    return rot_left(root);
}

AVLNode* avl_fix(AVLNode* node) {
    while (true) {
        avl_update(node);
        uint32_t l = avl_depth(node->m_left);
        uint32_t r = avl_depth(node->m_right);
        AVLNode** from { nullptr };
        if (AVLNode* p = node->m_parent) {
            from = (p->m_left == node) ? &p->m_left : &p->m_right;
        }

        if (l == r + 2) {
            node = avl_fix_left(node);
        } else if (l + 2 == r) {
            node = avl_fix_right(node);
        }

        if (!from) {
            return node;
        }

        *from = node;
        node = node->m_parent;
    }
}

// 笑死，完全不会
// 分离节点并且返回树根
AVLNode* avl_del(AVLNode* node) {
    if (node->m_right == nullptr) {
        AVLNode* parent = node->m_parent;
        if (node->m_left) {
            node->m_left->m_parent = parent;
        }
        if (parent) {
            (parent->m_left == node ? parent->m_left : parent->m_right) = node->m_left;
            return avl_fix(parent);
        } else {
            return node->m_left;
        }
    } else {
        AVLNode* victim = node->m_right;
        while (victim->m_left) {
            victim = victim->m_left;
        }
        AVLNode* root = avl_del(victim);
        *victim = *node;
        if (victim->m_left) {
            victim->m_left->m_parent = victim;
        }
        if (victim->m_right) {
            victim->m_right->m_parent = victim;
        }
        if (AVLNode* parent = node->m_parent) {
            (parent->m_left == node ? parent->m_left : parent->m_right) = victim;
            return root;
        } else {
            return victim;
        }
    }
}

AVLNode* avl_offset(AVLNode* node, int64_t offset) {
    int64_t pos { 0 };
    while (offset != pos) {
        if (pos < offset & pos + avl_cnt(node->m_right) >= offset) {
            node = node->m_right;
            pos += avl_cnt(node->m_left) + 1;
        } else if (pos > offset & pos - avl_cnt(node->m_left) <= offset) {
            node = node->m_left;
            pos -= avl_cnt(node->m_right) + 1;
        } else {
            AVLNode* parent = node->m_parent;
            if (!parent) {
                return nullptr;
            }

            if (parent->m_right == node) {
                pos -= avl_cnt(node->m_left) + 1;
            } else {
                pos += avl_cnt(node->m_right) + 1;
            }

            node = parent;
        }
    }

    return node;
}

void Container::add(uint32_t val) {
    Data* data = new Data();
    avl_init(&data->m_node);
    data->val = val;

    AVLNode* cur { nullptr };
    AVLNode** from = &m_root;
    while (*from) {
        cur = *from;
        uint32_t node_val = container_of(cur, SimpleRedis::Data, m_node)->val;
        from = (val < node_val) ? &cur->m_left : &cur->m_right;
    }

    *from = &data->m_node;
    data->m_node.m_parent = cur;
    this->m_root = avl_fix(&data->m_node);
}

bool Container::del(uint32_t val) {
    AVLNode* cur = this->m_root;
    while (cur) {
        uint32_t node_val = container_of(cur, Data, m_node)->val;
        // 找到了
        if (val == node_val) {
            break;
        }
        cur = val < node_val ? cur->m_left : cur->m_right;
    }

    if (!cur) {
        return false;
    }

    m_root = avl_del(cur);
    delete container_of(cur, Data, m_node);
    return true;
}

void dispose(Container& container) {
    while (container.m_root) {
        AVLNode* node = container.m_root;
        container.m_root = avl_del(container.m_root);
        delete container_of(node, SimpleRedis::Data, m_node);
    }
}
} // namespace SimpleRedis