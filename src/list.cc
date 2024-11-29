#include "list.h"

namespace SimpleRedis {
void list_init(List* node) { node->pre = node->next = node; }

bool list_empty(List* node) { return node->next = node; }

void list_detach(List* node) {
    List* prev = node->pre;
    List* nextv = node->next;
    prev->next = nextv;
    nextv->pre = prev;
}

void list_insert_before(List* target, List* rookie);
} // namespace SimpleRedis