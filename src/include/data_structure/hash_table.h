/**
 * @file hash_table.h
 * @author your name (you@domain.com)
 * @brief  底层数据结构不适合使用智能指针进行封装，而应该使用裸指针，裸指针就行了
 * @version 0.1
 * @date 2024-08-04
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <string>
#include <vector>

namespace SimpleRedis {

constexpr size_t k_resizing_work { 128 };
constexpr size_t k_max_load_factor { 8 };

struct HNode {
    HNode* next { nullptr };
    uint64_t hcode { 0 };
    HNode() = default;
    HNode(uint64_t hcode_) : next { nullptr }, hcode { hcode_ } {}
    HNode(const HNode&) = delete;
    HNode(HNode&&) = delete;
    HNode& operator=(const HNode&) = delete;
    HNode& operator=(HNode&&) = delete;
    // 在侵入式数据结构中这里的实现是错误的,不能这样实现
    // ~HNode() {
    //     if (next) {
    //         delete next;
    //         next = nullptr;
    //     }
    // }
};

using Cmp = std::function<bool(HNode*, HNode*)>;

struct HTab {
public:
    explicit HTab(size_t n_) {
        assert(n_ > 0 && ((n_ - 1) & n_) == 0);
        tab = (HNode**)calloc(sizeof(HNode*), n_);
        mask = n_ - 1;
        size = 0;
    }

    HTab() = default;
    HTab(const HTab&) = delete;
    // HTab(HTab&&) = default;
    // HTab& operator=(const HTab&) = delete;
    // HTab& operator=(HTab&&) = delete;

    void insert(HNode* node) {
        size_t pos = node->hcode & mask;
        HNode* next = tab[pos];

        node->next = next;
        tab[pos] = node;
        size++;
    }

    HNode** lookup(const HNode* key, Cmp cmp) {
        if (!tab) {
            return nullptr;
        }

        size_t pos = key->hcode & mask;
        HNode** from = &tab[pos];

        while (from != nullptr) {
            if ((*from)->hcode == key->hcode && cmp(key, *from)) {
                return from;
            }
            from = &(*from)->next;
        }
        return nullptr;
    }

    // 只要知道这个地址就可以了
    HNode* detach(HNode** from) {
        HNode* node = *from;
        *from = node->next;
        size--;
        return node;
    }

    HNode** tab = { nullptr }; // 其实就是array，
    size_t mask = { 0 };       // 掩码，2^n - 1;
    size_t size = { 0 };
};

struct HMap {
public:
    void insert(HNode* node) {
        if (!ht1.tab) {
            ht1 = HTab(4);
        }
        ht1.insert(node);

        if (!ht2.tab) {
            size_t load_factor = ht1.size / (ht1.mask + 1);
            if (load_factor >= k_max_load_factor) {
                start_resizing();
            }
        }
        help_resizing();
    }

    HNode* lookup(HNode* key, Cmp cmp) {
        help_resizing();
        HNode** from = ht1.lookup(key, cmp);
        from = from ? from : ht2.lookup(key, cmp);
        return from ? *from : nullptr;
    }

private:
    void start_resizing() {
        assert(ht2.tab == nullptr);
        ht2.tab = ht1.tab;
        ht1 = HTab((ht1.mask + 1) * 2);
        resizing_pos = 0;
    }

    // 移动到新表中
    void help_resizing() {
        size_t nwork { 0 };
        while (nwork < k_resizing_work && ht2.size > 0) {
            HNode** from = &ht2.tab[resizing_pos];
            if (!*from) {
                resizing_pos++;
                continue;
            }
            ht1.insert(ht2.detach(from));
            nwork++;
        }
        if (ht2.size == 0 && ht2.tab) {
            free(ht2.tab);
            ht2 = HTab {};
        }
    }
    HTab ht1 {};
    HTab ht2 {};
    size_t resizing_pos { 0 };
};

struct Entry {
    HNode node;
    std::string key {};
    std::string val;
};

} // namespace SimpleRedis

// #pragma once

// #include <functional>
// #include <memory>
// #include <utility>
// #include <vector>
// #include <iostream>
// #include <format>
// #include <cassert>

// const size_t k_resizing_work = 128;
// const size_t k_max_load_factor = 8;

// namespace SimpleRedis {

// struct HNode {
// public:
//     HNode() = default;
//     HNode(long long hCode) : hCode { hCode }, next { nullptr } {}
//     std::unique_ptr<HNode> next { nullptr };
//     long long hCode { 0 };
// };

// using Cmp =
//     std::function<bool(const std::unique_ptr<HNode>&, const std::unique_ptr<HNode>&)>;

// struct HashTable {
// public:
//     HashTable() = default;
//     HashTable(size_t n) : mask { n - 1 }, tab(n), size { 0 } {
//         std::cout << std::format("HashTable n = {}", n) << std::endl;
//         assert(n > 0 && ((n - 1) & n) == 0);
//     }

//     // 插入一个数值
//     void insert(std::unique_ptr<HNode> node) {
//         size_t pos = node->hCode & mask;
//         std::unique_ptr<HNode>& next = tab[pos];
//         node->next = std::move(next);
//         tab[pos] = std::move(node);
//         size++;
//     }

//     // 这个返回值有点怪，但是感觉这是最好的一种方式了,因为可能是空的
//     std::unique_ptr<HNode>* lookup(const std::unique_ptr<HNode>& key, Cmp cmp) {
//         if (tab.empty()) {
//             return nullptr;
//         }

//         size_t pos = key->hCode & mask;

//         std::unique_ptr<HNode>* cur = &tab[pos];

//         while (cur != nullptr) {
//             if (cmp(*cur, key)) {
//                 return cur;
//             }
//             cur = &((*cur)->next);
//         }
//         return nullptr;
//     }

//     // 奇怪。。
//     std::unique_ptr<HNode> detach(std::unique_ptr<HNode>& from) {
//         std::unique_ptr<HNode> node = std::move(from);
//         from = std::move((*from).next);
//         size--;
//         return node;
//     }

// public:
//     std::vector<std::unique_ptr<HNode>> tab {}; // 相当于一个数组的头，约等于 HNode**
//     size_t mask {};                             // 掩码
//     size_t size {};                             // 大小
// };

// class HashMap {
// public:
//     HashMap() = default;
//     std::unique_ptr<HNode> lookup(const std::unique_ptr<HNode>& key, Cmp cmp) {
//         help_resizing();
//         // 这个地方因为可能是空的
//         std::unique_ptr<HNode>* from = nullptr;
//         from = std::move(ht1.lookup(key, cmp));
//         if (!from) {
//             from = std::move(ht2.lookup(key, cmp));
//         }

//         if (from) {
//             return std::move(*from);
//         } else {
//             return nullptr;
//         }
//     }

//     // 插入数据
//     void insert(std::unique_ptr<HNode> node) {
//         if (!ht1.tab.size()) {
//             ht1 = HashTable(4);
//         }
//         ht1.insert(std::move(node));

//         if (!ht2.tab.size()) {
//             size_t load_factor = ht1.size / (ht1.mask + 1);
//             if (load_factor >= k_max_load_factor) {
//                 start_resizing();
//             }
//         }

//         help_resizing();
//     }

//     std::unique_ptr<HNode> pop(std::unique_ptr<HNode>& key, Cmp cmp) {
//         help_resizing();
//         auto from = ht1.lookup(key, cmp);
//         if (from) {
//             return ht1.detach(*from);
//         }
//         from = ht2.lookup(key, cmp);
//         if (from) {
//             return ht2.detach(*from);
//         }
//     }

// private:
//     void start_resizing() {
//         assert(ht2.tab.size() == 0);
//         ht2 = std::move(ht2);
//         ht1 = HashTable(4);
//         resizing_pos = 0;
//     }

//     void help_resizing() {
//         if (!ht2.tab.size()) {
//             return;
//         }

//         size_t nwork = 0;
//         while (nwork < k_resizing_work && ht2.size > 0) {
//             std::unique_ptr<HNode>* from = &ht2.tab[resizing_pos];
//             if (!*from) {
//                 resizing_pos ++;
//                 continue;
//             }
//             ht1.insert(ht2.detach(*from));
//             nwork++;
//         }
//         if (ht2.size == 0) ht2 = std::move(HashTable{});
//     }

// public:
//     HashTable ht1 {};
//     HashTable ht2 {};
//     size_t resizing_pos { 0 };
// };
// } // namespace SimpleRedis