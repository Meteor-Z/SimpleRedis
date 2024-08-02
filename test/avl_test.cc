#include <iostream>
#include <memory>
#include "data_structure/avl.h"

int main() {
    std::unique_ptr<SimpleRedis::AvlNode> root = std::make_unique<SimpleRedis::AvlNode>();
    SimpleRedis::insert(root, 1);
    SimpleRedis::insert(root,29);
    SimpleRedis::insert(root, 39);
    SimpleRedis::insert(root, 20);
    SimpleRedis::insert(root, -1);
    SimpleRedis::insert(root, 30);

    SimpleRedis::preoder_traversal(root);
}