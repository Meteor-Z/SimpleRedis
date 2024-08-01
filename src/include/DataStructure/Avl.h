#pragma once

#include <algorithm>
#include <cstddef>
#include <ctime>
#include <endian.h>
#include <iostream>
#include <memory>
#include <queue>
#include <ratio>
#include <utility>

namespace SimpleRedis {
struct AvlNode {
    AvlNode(const int& x, const int& y, std::unique_ptr<AvlNode>&& p = nullptr,
            std::unique_ptr<AvlNode>&& q = nullptr)
        : data(x), height(y), left(std::move(p)), right(std::move(q)) {}

    AvlNode(int data_) : data(data_), height(1) {}
    AvlNode() : height { 1 } {}

    std::unique_ptr<AvlNode> left { nullptr };
    std::unique_ptr<AvlNode> right { nullptr };
    size_t height { 0 }; // 子树的高度
    int data { 0 };      // 数据
};

// inline std::unique_ptr<AvlNode> g_root = nullptr;

inline size_t get_height(std::unique_ptr<AvlNode>& root) {
    if (!root) {
        return 0;
    }
    return root->height;
}

inline int height_diff(std::unique_ptr<AvlNode>& root) {
    if (!root) {
        return 0;
    }
    return get_height(root->left) - get_height(root->right);
}

// 这里旋转的时候，还要搞一下
inline void left_rotate(std::unique_ptr<AvlNode>& node) {
    std::unique_ptr<AvlNode> y = std::move(node->right);
    std::unique_ptr<AvlNode> T2 = std::move(node->right);

    // 开始旋转！
    y->left = std::move(node);
    y->left->right = std::move(T2);

    y->left->height = std::max(get_height(y->left->left), get_height(y->left->right)) + 1;
    y->height = std::max(get_height(y->right), get_height(y->right)) + 1;
}

inline void right_rotate(std::unique_ptr<AvlNode>& node) {
    std::unique_ptr<AvlNode> x = std::move(node->left);
    std::unique_ptr<AvlNode> T2 = std::move(x->right);

    // Perform rotation
    x->right = std::move(node);
    x->right->left = std::move(T2);

    // Update heights
    x->right->height =
        std::max(get_height(x->right->left), get_height(x->right->right)) + 1;
    x->height = std::max(get_height(x->left), get_height(x->right)) + 1;
}

inline auto find_min(std::unique_ptr<AvlNode>& root) -> AvlNode* {
    while (root->left != nullptr)
        root = std::move(root->left);
    return root.get();
}

inline auto insert(std::unique_ptr<AvlNode>& root, int data) -> void {
    std::unique_ptr<AvlNode> newData = std::make_unique<AvlNode>(data);

    if (root == nullptr) {
        root = std::move(newData);
        return;
    } else if (data < root->data) {
        insert(root->left, data);
    } else {
        insert(root->right, data);
    }

    root->height = 1 + std::max(get_height(root->left), get_height(root->right));

    int balance = height_diff(root);

    if (balance > 1 && root->left && data < root->left->data) {
        right_rotate(root);
    }

    if (balance < -1 && root->right && data > root->right->data) {
        left_rotate(root);
    }

    if (balance > 1 && root->left && data > root->left->data) {
        left_rotate(root->left);
        right_rotate(root);
    }

    // Right Left Case
    if (balance < -1 && root->right && data < root->right->data) {
        right_rotate(root->right);
        left_rotate(root);
    }
}

// 中序遍历
inline auto inorder_traversal(std::unique_ptr<AvlNode>& root) -> void {
    if (!root) {
        inorder_traversal(root->left);
        std::cout << root->data << " ";
        inorder_traversal(root->right);
    }
}

inline auto preoder_traversal(std::unique_ptr<AvlNode>& root) -> void {
    if (root != nullptr) {
        std::cout << root->data << " ";
        preoder_traversal(root->left);
        preoder_traversal(root->right);
    }
}

inline auto postorder_traversal(std::unique_ptr<AvlNode>& root) -> void {
    if (root != nullptr) {
        postorder_traversal(root->left);
        postorder_traversal(root->right);
        std::cout << root->data << " ";
    }
}

// 这里学到了，因为这里使用了智能指针，所以使用这个是非常不错的
inline auto bfs(std::unique_ptr<AvlNode>& root) -> void {
    std::queue<const AvlNode*> queue{};
    queue.push(root.get());
    
    while (!queue.empty()) {
        auto t = queue.front();
        queue.pop();
        if (t->left != nullptr) queue.push(t->left.get());
        if (t->right != nullptr) queue.push(t->right.get());
        
        std::cout << t->data << " ";
    }
}

inline auto dfs(std::unique_ptr<AvlNode>& root) -> void {
    if (!root) {
        return;
    }

    dfs(root->left);
    dfs(root->right);
    std::cout << root->data << " ";
    
}

inline auto exists(std::unique_ptr<AvlNode>& root, int value) -> bool {
    auto temp = root.get();
    while (temp != nullptr) {
        if (temp->data == value) {
            return true;
        } else {
            if (value > temp->data) {
                temp = temp->right.get();
            } else {
                temp = temp->right.get();
            }
        }
    }

    return false;
}
} // namespace SimpleRedis