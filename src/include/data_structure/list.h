#pragma once

namespace SimpleRedis {

struct List {
    List* pre { nullptr };
    List* next { nullptr };
    List() : pre { this }, next { this } {}
    bool empty() const noexcept { return next == this; }

    void detach() const {
        auto* p = pre;
        auto* n = next;
        p->next = n;
        n->pre = p;
    }

    void insert_before(List* list) {
        List* t = pre;
        t->next = list;
        list->pre = pre;
        list->next = this;
        this->pre = list;
    }
};
}; // namespace SimpleRedis