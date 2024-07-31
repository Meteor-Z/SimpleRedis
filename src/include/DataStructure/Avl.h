#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <ratio>

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

inline std::unique_ptr<AvlNode> g_root = nullptr;

inline size_t get_depth(std::unique_ptr<AvlNode>& root) {
    if (!root) {
        return 0;
    }
    return root->height;
}

inline int height_diff(std::unique_ptr<AvlNode>& root) {
    if (!root) {
        return 0;
    }
    return get_depth(root->left) - get_depth(root->right);
}

inline void insert(std::unique_ptr<AvlNode>& root, int data) {
    std::unique_ptr<AvlNode> newData = std::make_unique<AvlNode>(data);

    if (root == nullptr) {
        root = std::move(newData);
        return;
    } else if (data < root->data) {
        insert(root->left, data);
    } else {
        insert(root->right, data);
    }

    root->height = 1 + std::max(get_depth(root->left), get_depth(root->right));

    int balance = height_diff(root);

    if (balance > 1 && root->left && data < root->left->data) {
        
    }
}
} // namespace SimpleRedis