/**
 * @file heap.h
 * @author lzc (liuzechen.coder@qq.com)
 * @brief Heap，设置TTL(生存事件)做缓存服务器的
 * @version 0.1
 * @date 2024-08-03
 *
 * @copyright Copyright (c) 2024
 *
 */

/*
缓存服务器用的，也就是TTL

*/
#pragma once

#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cfloat>
#include <cstddef>
#include <cstdlib>
#include <format>
#include <ostream>
#include <vector>

namespace SimpleRedis {
struct HeapItem {
public:
    HeapItem() = default;
    HeapItem(long long val_, size_t* ref_) : value { val_ }, ref { ref_ } {}
    // HeapItem(const HeapItem&) = delete;
    // HeapItem(HeapItem&&) = delete;
    // HeapItem& operator=(const HeapItem&) = delete;
    // HeapItem& operator=(HeapItem&&) = delete;

    friend bool operator<(const HeapItem& a, const HeapItem& b) {
        return a.value < b.value;
    }

    friend bool operator>(const HeapItem& a, const HeapItem& b) {
        return a.value > b.value;
    }

    friend bool operator<=(const HeapItem& a, const HeapItem& b) {
        return a.value <= b.value;
    }

    friend bool operator>=(const HeapItem& a, const HeapItem& b) {
        return a.value >= b.value;
    }

    // 用于打印的
    friend std::ostream& operator<<(std::ostream&& os, const HeapItem& heapItem) {
        os << std::format("HeapItem: value = {}, ref = {}", heapItem.value,
                          *heapItem.ref);
        return os;
    }

    void change_ref(size_t pos) const { *ref = pos; }

public:
    long long value { 0 };
    size_t* ref { nullptr };
};

struct Heap {
public:
    // Heap从1开始实现是比较好实现的，因为
    Heap() : data { 1 } {}
    friend std::ostream& operator<<(std::ostream& os, const Heap& heap) {
        os << "Healp:";
        // for (const auto& item : heap.data) {
        //     os << " " << item << " ";
        // }
        return os;
    }
    bool empty() { return data.size() == 1; }

    size_t* get_min_ref() {
        assert(!empty());
        return data[1].ref;
    }

    auto del(size_t pos) {
        data[pos] = data.back();
        data.pop_back();
        update(pos);
    }

    auto get(size_t pos) -> long long {
        assert(!empty());
        assert(1 <= pos && pos < data.size());
        return data[pos].value;
    }
    auto get_min() -> long long {
        assert(!empty());
        return data[1].value;
    }

    void push(long long value, size_t* entry_heap_idx) {
        data.emplace_back(HeapItem { value, entry_heap_idx });
        update(data.size() - 1);
    }

    void set(size_t pos, long long value) {
        data[pos].value = value;
        update(pos);
    }

    // 检查，这里主要是检查左右两边的数值
    bool check(size_t pos = 1) {
        bool flag = true;
        if ((pos << 1) < data.size()) {
            assert(data[pos << 1] >= data[pos]);
            flag &= check(pos << 1);
        }
        if ((pos << 1 | 1) < data.size()) {
            assert(data[pos << 1 | 1] >= data[pos]);
            flag &= check(pos << 1 | 1);
        }
        return flag;
    }

private:
    void update(size_t pos) {
        if (pos > 1 && data[pos >> 1] > data[pos]) {
            up(pos);
        } else {
            down(pos);
        }
    }

    void up(size_t now) {
        assert(1 <= now && now < data.size());

        size_t next;

        while (now > 1) {
            next = now >> 1;
            if (data[next] <= data[now]) {
                break;
            }
            std::swap(data[next], data[now]);
        }
        data[now].change_ref(now);
    }

    void down(size_t now) {
        assert(1 <= now && now < data.size());
        std::size_t next;
        while ((now << 1) < data.size()) {
            next = now << 1;
            if (next < data.size() - 1 && data[next + 1] < data[next]) {
                ++next;
            }

            if (data[now] < data[next]) {
                break;
            }
            std::swap(data[now], data[next]);
            now = next;
        }
        data[now].change_ref(now);
    }

private:
    std::vector<HeapItem> data {};
};

} // namespace SimpleRedis