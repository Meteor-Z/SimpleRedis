#pragma once

#include <algorithm>
#include <cstddef>

namespace SimpleRedis {
struct AvlNode {

    explicit AvlNode()
        : left { nullptr }, right { nullptr }, parent { nullptr }, depth { 1 },
          cnt { 1 } {}
    ~AvlNode() {
        if (left) {
            delete left;
            left = nullptr;
        }

        if (right) {
            delete right;
            right = nullptr;
        }
    }

    size_t depth { 0 };
    size_t cnt { 0 };
    AvlNode* left { nullptr };
    AvlNode* right { nullptr };
    AvlNode* parent { nullptr };
};

inline auto avl_get_cnt(const AvlNode* node) -> size_t { return node ? node->cnt : 0; }

inline auto avl_get_depth(const AvlNode* node) -> size_t {
    return node ? node->depth : 0;
}

inline auto avl_update(AvlNode* node) -> void {
    node->depth = std::max(avl_get_depth(node->left), avl_get_depth(node->right)) + 1;
    node->cnt = avl_get_cnt(node->left) + avl_get_cnt(node->right) + 1;
}

//   2           4
//  / \         / \
// 1   4  ==>  2   5
//    / \     / \
//   3   5   1   3
// 这里 2 是node, 4 是 new_node
inline auto avl_left_rot(AvlNode* node) -> AvlNode* {
    AvlNode* new_node = node->right;
    if (new_node->left) {
        // 3 - > 2
        new_node->left->parent = node;
    }
    // 开始旋转
    node->right = new_node->left;
    new_node->left = node;
    new_node->parent = node->parent;
    node->parent = new_node;

    avl_update(node);
    avl_update(new_node);
    //  返回新的 root
    return new_node;
}

inline auto avl_right_rot(AvlNode* node) -> AvlNode* {
    AvlNode* new_node = node->left;
    if (new_node->right) {
        new_node->right->parent = node;
    }
    node->left = new_node->right;
    new_node->right = node;
    new_node->parent = node->parent;
    node->parent = new_node;
    avl_update(node);
    avl_update(new_node);
    return new_node;
}

// 修复左旋
inline auto avl_fix_left(AvlNode* root) -> AvlNode* {
    if (avl_get_depth(root->left->left) < avl_get_depth(root->left->right)) {
        root->left = avl_left_rot(root->left);
    }
    return avl_right_rot(root);
}

inline auto avl_fix_right(AvlNode* root) -> AvlNode* {
    if (avl_get_depth(root->right->right) < avl_get_depth(root->right->left)) {
        root->right = avl_right_rot(root->right);
    }
    return avl_left_rot(root);
}

inline auto avl_fix(AvlNode* node) -> AvlNode* {
    while (true) {
        avl_update(node);
        size_t l = avl_get_depth(node->left);
        size_t r = avl_get_depth(node->right);
        AvlNode** from = nullptr;
        if (node->parent) {
            from =
                (node->parent->left == node) ? &node->parent->left : &node->parent->right;
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
        node = node->parent;
    }
}

// 返回的是新的数值 AvlNode*
inline auto avl_del(AvlNode* node) -> AvlNode* {
    if (node->right == NULL) {
        // no right subtree, replace the node with the left subtree
        // link the left subtree to the parent
        AvlNode* parent = node->parent;
        if (node->left) {
            node->left->parent = parent;
        }
        if (parent) {
            // attach the left subtree to the parent
            (parent->left == node ? parent->left : parent->right) = node->left;
            return avl_fix(parent);
        } else {
            // removing root?
            return node->left;
        }
    } else {
        // swap the node with its next sibling
        AvlNode* victim = node->right;
        while (victim->left) {
            victim = victim->left;
        }
        AvlNode* root = avl_del(victim);

        *victim = *node;
        if (victim->left) {
            victim->left->parent = victim;
        }
        if (victim->right) {
            victim->right->parent = victim;
        }
        AvlNode* parent = node->parent;
        if (parent) {
            (parent->left == node ? parent->left : parent->right) = victim;
            return root;
        } else {
            return victim;
        }
    }
}

inline AvlNode* avl_offset(AvlNode* node, int64_t offset) {
    int64_t pos = 0;
    while (offset != pos) {
        if (pos < offset && pos + avl_get_cnt(node->right) >= offset) {
            node = node->right;
            pos += avl_get_cnt(node->left) + 1;
        } else if (pos > offset && pos - avl_get_cnt(node->left) <= offset) {
            node = node->left;
            pos -= avl_get_cnt(node->right) + 1;
        } else {
            // go to the parent
            AvlNode* parent = node->parent;
            if (!parent) {
                return nullptr;
            }
            if (parent->right == node) {
                pos -= avl_get_cnt(node->left) + 1;
            } else {
                pos += avl_get_cnt(node->right) + 1;
            }
            node = parent;
        }
    }
    return node;
}

// 对本身进行free操作, 侵入式数据结构是没有这个的!
// inline auto avl_free(AvlNode* root) -> void {
//     if (!root) {
//         return;
//     }

//     avl_free(root->left);
//     avl_free(root->right);

//     if (root) {
//         delete root;
//         root = nullptr;
//     }
// }

}; // namespace SimpleRedis

// #pragma once

// #include <algorithm>
// #include <cstddef>
// #include <ctime>
// #include <endian.h>
// #include <iostream>
// #include <memory>
// #include <queue>
// #include <utility>

// namespace SimpleRedis {
// struct AvlNode {
//     AvlNode(const int& x, const int& y, std::unique_ptr<AvlNode>&& p = nullptr,
//             std::unique_ptr<AvlNode>&& q = nullptr)
//         : data(x), height(y), left(std::move(p)), right(std::move(q)) {}

//     AvlNode(int data_) : data(data_), height(1) {}
//     AvlNode() : height { 1 } {}

//     std::unique_ptr<AvlNode> left { nullptr };
//     std::unique_ptr<AvlNode> right { nullptr };
//     size_t height { 0 }; // 子树的高度
//     int data { 0 };      // 数据
// };

// // inline std::unique_ptr<AvlNode> g_root = nullptr;

// inline size_t get_height(std::unique_ptr<AvlNode>& root) {
//     if (!root) {
//         return 0;
//     }
//     return root->height;
// }

// inline int height_diff(std::unique_ptr<AvlNode>& root) {
//     if (!root) {
//         return 0;
//     }
//     return get_height(root->left) - get_height(root->right);
// }

// // 这里旋转的时候，还要搞一下
// inline void left_rotate(std::unique_ptr<AvlNode>& node) {
//     std::unique_ptr<AvlNode> y = std::move(node->right);
//     std::unique_ptr<AvlNode> T2 = std::move(node->right);

//     // 开始旋转！
//     y->left = std::move(node);
//     y->left->right = std::move(T2);

//     y->height = std::max(get_height(y->left), get_height(y->right)) + 1;
//     node = std::move(y);
// }

// inline void right_rotate(std::unique_ptr<AvlNode>& node) {
//     std::unique_ptr<AvlNode> x = std::move(node->left);
//     std::unique_ptr<AvlNode> T2 = std::move(x->right);

//     // Perform rotation
//     x->right = std::move(node);
//     x->right->left = std::move(T2);

//     // Update heights
//     x->height = std::max(get_height(node->left), get_height(x->right)) + 1;
//     node = std::move(x);
// }

// inline auto find_min(std::unique_ptr<AvlNode>& root) -> const AvlNode* {
//     while (root->left != nullptr) {
//         root = std::move(root->left);
//     }

//     return root.get();
// }

// inline auto insert(std::unique_ptr<AvlNode>& root, int data) -> void {
//     std::unique_ptr<AvlNode> newData = std::make_unique<AvlNode>(data);

//     if (root == nullptr) {
//         root = std::move(newData);
//         return;
//     } else if (data < root->data) {
//         insert(root->left, data);
//     } else {
//         insert(root->right, data);
//     }

//     root->height = 1 + std::max(get_height(root->left), get_height(root->right));

//     int balance = height_diff(root);

//     if (balance > 1 && root->left && data < root->left->data) {
//         right_rotate(root);
//     }

//     if (balance < -1 && root->right && data > root->right->data) {
//         left_rotate(root);
//     }

//     if (balance > 1 && root->left && data > root->left->data) {
//         left_rotate(root->left);
//         right_rotate(root);
//     }

//     // Right Left Case
//     if (balance < -1 && root->right && data < root->right->data) {
//         right_rotate(root->right);
//         left_rotate(root);
//     }
// }

// // 中序遍历
// inline auto inorder_traversal(std::unique_ptr<AvlNode>& root) -> void {
//     if (!root) {
//         inorder_traversal(root->left);
//         std::cout << root->data << " ";
//         inorder_traversal(root->right);
//     }
// }

// inline auto preoder_traversal(std::unique_ptr<AvlNode>& root) -> void {
//     if (root != nullptr) {
//         std::cout << root->data << " ";
//         preoder_traversal(root->left);
//         preoder_traversal(root->right);
//     }
// }

// inline auto postorder_traversal(std::unique_ptr<AvlNode>& root) -> void {
//     if (root != nullptr) {
//         postorder_traversal(root->left);
//         postorder_traversal(root->right);
//         std::cout << root->data << " ";
//     }
// }

// // 这里学到了，因为这里使用了智能指针，所以使用这个是非常不错的
// inline auto bfs(const std::unique_ptr<AvlNode>& root) -> void {
//     std::queue<const AvlNode*> queue {};
//     queue.push(root.get());

//     while (!queue.empty()) {
//         auto t = queue.front();
//         queue.pop();
//         if (t->left != nullptr) {
//             queue.push(t->left.get());
//         }
//         if (t->right != nullptr) {
//             queue.push(t->right.get());
//         }

//         std::cout << t->data << " ";
//     }
// }

// inline auto dfs(std::unique_ptr<AvlNode>& root) -> void {
//     if (!root) {
//         return;
//     }

//     dfs(root->left);
//     dfs(root->right);
//     std::cout << root->data << " ";
// }

// inline auto exists(std::unique_ptr<AvlNode>& root, int value) -> bool {
//     auto temp = root.get();
//     while (temp != nullptr) {
//         if (temp->data == value) {
//             return true;
//         } else {
//             if (value > temp->data) {
//                 temp = temp->right.get();
//             } else {
//                 temp = temp->right.get();
//             }
//         }
//     }

//     return false;
// }
// } // namespace SimpleRedis