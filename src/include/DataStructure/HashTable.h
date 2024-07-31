#pragma once

#include <functional>
#include <memory>
#include <utility>
#include <vector>
#include <iostream>
#include <format>
#include <cassert>

const size_t k_resizing_work = 128;
const size_t k_max_load_factor = 8;

namespace SimpleRedis {

struct HNode {
public:
    HNode() = default;
    HNode(long long hCode) : hCode { hCode }, next { nullptr } {}
    std::unique_ptr<HNode> next { nullptr };
    long long hCode { 0 };
};

using Cmp =
    std::function<bool(const std::unique_ptr<HNode>&, const std::unique_ptr<HNode>&)>;

struct HashTable {
public:
    HashTable() = default;
    HashTable(size_t n) : mask { n - 1 }, tab(n), size { 0 } {
        std::cout << std::format("HashTable n = {}", n) << std::endl;
        assert(n > 0 && ((n - 1) & n) == 0);
    }

    // 插入一个数值
    void insert(std::unique_ptr<HNode> node) {
        size_t pos = node->hCode & mask;
        std::unique_ptr<HNode>& next = tab[pos];
        node->next = std::move(next);
        tab[pos] = std::move(node);
        size++;
    }

    // 这个返回值有点怪，但是感觉这是最好的一种方式了,因为可能是空的
    std::unique_ptr<HNode>* lookup(const std::unique_ptr<HNode>& key, Cmp cmp) {
        if (tab.empty()) {
            return nullptr;
        }

        size_t pos = key->hCode & mask;

        std::unique_ptr<HNode>* cur = &tab[pos];

        while (cur != nullptr) {
            if (cmp(*cur, key)) {
                return cur;
            }
            cur = &((*cur)->next);
        }
        return nullptr;
    }

    // 奇怪。。
    std::unique_ptr<HNode> detach(std::unique_ptr<HNode>& from) {
        std::unique_ptr<HNode> node = std::move(from);
        from = std::move((*from).next);
        size--;
        return node;
    }

public:
    std::vector<std::unique_ptr<HNode>> tab {}; // 相当于一个数组的头，约等于 HNode**
    size_t mask {};                             // 掩码
    size_t size {};                             // 大小
};

class HashMap {
public:
    HashMap() = default;
    std::unique_ptr<HNode> lookup(const std::unique_ptr<HNode>& key, Cmp cmp) {
        help_resizing();
        // 这个地方因为可能是空的
        std::unique_ptr<HNode>* from = nullptr;
        from = std::move(ht1.lookup(key, cmp));
        if (!from) {
            from = std::move(ht2.lookup(key, cmp));
        }

        if (from) {
            return std::move(*from);
        } else {
            return nullptr;
        }
    }

    // 插入数据
    void insert(std::unique_ptr<HNode> node) {
        if (!ht1.tab.size()) {
            ht1 = HashTable(4);
        }
        ht1.insert(std::move(node));

        if (!ht2.tab.size()) {
            size_t load_factor = ht1.size / (ht1.mask + 1);
            if (load_factor >= k_max_load_factor) {
                start_resizing();
            }
        }

        help_resizing();
    }

    std::unique_ptr<HNode> pop(std::unique_ptr<HNode>& key, Cmp cmp) {
        help_resizing();
        auto from = ht1.lookup(key, cmp);
        if (from) {
            return ht1.detach(*from);
        }
        from = ht2.lookup(key, cmp);
        if (from) {
            return ht2.detach(*from);
        }
    }

private:
    void start_resizing() {
        assert(ht2.tab.size() == 0);
        ht2 = std::move(ht2);
        ht1 = HashTable(4);
        resizing_pos = 0;
    }
    
    void help_resizing() {
        if (!ht2.tab.size()) {
            return;
        }

        size_t nwork = 0;
        while (nwork < k_resizing_work && ht2.size > 0) {
            std::unique_ptr<HNode>* from = &ht2.tab[resizing_pos];
            if (!*from) {
                resizing_pos ++;
                continue;
            }
            ht1.insert(ht2.detach(*from));
            nwork++;
        }
        if (ht2.size == 0) ht2 = std::move(HashTable{});
    }

public:
    HashTable ht1 {};
    HashTable ht2 {};
    size_t resizing_pos { 0 };
};
} // namespace SimpleRedis