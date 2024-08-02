#pragma once

#include <memory>

namespace SimpleRedis {

struct ListNode {
    ListNode* pre { nullptr };
    ListNode* next { nullptr };
    
};
}; // namespace SimpleRedis