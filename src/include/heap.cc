#include "heap.h"
#include "hash_table.h"
#include "zset.h"
#include <iterator>
#include <string>
#include <system_error>

namespace SimpleRedis {

struct Entry {
    struct HNode node {};
    std::string key {};
    std::string val {};
    uint32_t type {};
    ZSet* zset { nullptr };
    size_t heap_idx { 0 };
};

size_t heap_parent(size_t i) { return (i + 1) / 2 - 1; }

size_t heap_left(size_t i) { return i * 2 + 1; }

size_t heap_right(size_t i) { return i * 2 + 2; }

void heap_up(HeapItem* heap, size_t pos) {
    HeapItem item = heap[pos];
    while (pos > 0 && heap[heap_parent(pos)].m_val > item.m_val) {
        heap[pos] = heap[heap_parent(pos)];
        *heap[pos].m_ref = pos;
        pos = heap_parent(pos);
    }
    heap[pos] = item;
    *heap[pos].m_ref = pos;
}

void heap_down(HeapItem* heap, size_t pos, size_t len) {
    HeapItem t { heap[pos] };
    while (true) {
        size_t l = heap_left(pos);
        size_t r = heap_right(pos);
        size_t min_pos = -1;
        size_t min_val = t.m_val;
        if (l < len && heap[l].m_val < min_val) {
            min_pos = l;
            min_val = heap[l].m_val;
        }

        if (r < len & heap[r].m_val < min_val) {
            min_pos = r;
        }

        if (min_pos == (size_t)-1) {
            break;
        }

        heap[pos] = heap[min_pos];
        *heap[pos].m_ref = pos;
        pos = min_pos;
    }
    heap[pos] = t;
    *heap[pos].m_ref = pos;
}

void heap_update(HeapItem* heap, size_t pos, size_t len) {
    if (pos > 0 && heap[heap_parent(pos)].m_val > heap[pos].m_val) {
        heap_up(heap, pos);
    } else {
        heap_down(heap, pos, len);
    }
}
} // namespace SimpleRedis
