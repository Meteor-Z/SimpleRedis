#pragma once

#include "avl.h"
#include "hash_table.h"

namespace SimpleRedis {

struct ZNode {
    AVLNode m_tree {}; // (score, name)
    HNode m_hmap {};
    double m_score { 0 };
    size_t m_len { 0 };
    char m_name[0]; // 可变长度结构 ？ 这个是什么
};

struct ZSet {
    ZNode* lookup(const char* name, size_t len);
    // 添加数据
    void add(ZNode* node);
    // 添加一个score, name
    bool add(const char* name, size_t len, double score);

    void update(ZNode* node, double score);
    // 查询这个ZNode, 然后detach他
    ZNode* pop(const char* name, size_t len);
    
    // 查询这个键值对
    ZNode* query(const char* name, double score, size_t len);

    AVLNode* m_tree { nullptr };
    HMap m_hmap {}; // KV存储的数据
};

ZNode* znode_new(const char* name, size_t len, double score);

void znode_del(ZNode* node);

// 便宜，这里主要是为了范围查询，也就是>=(score, name)的数据，比如说查询3个，那么offset就是3
ZNode* znode_offset(ZNode* node, int64_t offset);

} // namespace SimpleRedis