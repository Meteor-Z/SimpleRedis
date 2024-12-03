#pragma once

#include <cstddef>
#include <cstdint>

namespace SimpleRedis {
struct HeapItem {
    uint64_t m_val {};
    size_t* m_ref { nullptr }; // ref
};

size_t heap_parent(size_t i);

size_t heap_left(size_t i);

size_t heap_right(size_t i);

// 在这个位置上的pos向上走
void heap_up(HeapItem* heap, size_t pos);

// down一下，pos是当前的数据，len是堆的总长度
void heap_down(HeapItem* heap, size_t pos, size_t len);

// 更新数据，pos是当前的位置，len是堆的总长度
void heap_update(HeapItem* heap, size_t pos, size_t len);

} // namespace SimpleRedis