#pragma once

namespace SimpleRedis {
// 这里可以算是一个唤醒的链表
struct List {
    List* pre { nullptr };
    List* next { nullptr };
};

void list_init(List* node);

bool list_empty(List* node);

void list_detach(List* node);

void list_insert_before(List* target, List* rookie);
}; // namespace SimpleRedis