#pragma once

#include "DataStructure/Avl.h"
#include <memory>

namespace SimpleRedis {

struct ListNode {
public:
    ListNode() : pre { this }, next { this } {}
    ListNode(const ListNode&) = delete;
    ListNode(ListNode&&) = delete;
    ListNode& operator=(const ListNode&) = delete;
    ListNode& operator=(ListNode&&) = delete;

    ~ListNode() {
        if (pre) {
            delete pre;
        }
        if (next) {
            delete next;
        }
    }

    auto empty() const -> bool { return next == this; }

    // 将自身分离出来
    auto detach() const -> void {
        ListNode* p = pre;
        ListNode* n = next;
        p->next = n;
        n->next = p;
    }

public:
    ListNode* pre { nullptr };
    ListNode* next { nullptr };
};
}; // namespace SimpleRedis