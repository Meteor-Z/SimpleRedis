#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>


namespace SimpleRedis {

struct HNode {
    HNode* m_next { nullptr };
    uint64_t m_code { 0 }; // 其实这个就是str, 只不过str_hash了一成数字了
};

// hash表，这里主要是存储的kv，因为直接存储str不方便，那么这里直接存储的是h_code, 也就是字符串hash一下
struct HTab {
public:
    using Cmp = std::function<bool(HNode*, HNode*)>;

public:
    HTab() = default;
    HTab(const size_t n);

    void insert(HNode* node);

    // 查询匹配的HNode, 返回的是对应的地址
    HNode** lookup(HNode* key, Cmp cmp);

    HNode* detach(HNode** from);

    HNode** m_tab { nullptr }; // 相当于slot，槽
    size_t m_mask { 0 };       // 2^n - 1，掩码
    size_t m_size { 0 };       // 数量
};

struct HMap {
public:
    using Cmp = std::function<bool(HNode*, HNode*)>;
    static constexpr size_t MAX_LOAD_FACTOR { 8 };
    static constexpr size_t MAX_RESIZING_WORK { 128 };

public:
    void insert(HNode* node);

    HNode* lookup(HNode* node, Cmp cmp);

private:
    void start_resizing();

    void help_resizing();

public:
    HTab m_tab1; // 新表
    HTab m_tab2; // 旧表
    size_t m_resizing_pos = 0;
};


} // namespace SimpleRedis