#include "zset.h"
#include "avl.h"
#include "hash_table.h"
#include "tool.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>

namespace SimpleRedis {

struct Entry {
public:
    HNode m_node {};
    std::string m_key {};
    uint32_t type = 0;
    std::string val {};
    ZSet* zset { nullptr };
};

struct HKey {
    HNode node {};
    const char* name { nullptr };
    size_t len {};
};
ZNode* znode_new(const char* name, size_t len, double score) {
    // len 是最后的char的数组
    ZNode* node = (ZNode*)malloc(sizeof(ZNode) + len);
    avl_init(&node->m_tree);

    node->m_hmap.m_next = nullptr;
    node->m_hmap.m_code =
        str_hash(reinterpret_cast<uint8_t*>(const_cast<char*>(name)), len);
    node->m_score = score;
    node->m_len = len;

    // 这个牛逼
    std::memcpy(&node->m_name[0], name, len);
    return node;
}

void znode_del(ZNode* node) { free(node); }

ZNode* znode_offset(ZNode* node, int64_t offset) {
    AVLNode* tnode = node ? avl_offset(&node->m_tree, offset) : nullptr;
    return tnode ? container_of(tnode, ZNode, m_tree): nullptr;
}
bool hcmp(HNode* node, HNode* key) {
    ZNode* znode = container_of(node, ZNode, m_hmap);
    HKey* hkey = container_of(key, HKey, node);
    if (znode->m_len != hkey->len) {
        return false;
    }
    return memcmp(znode->m_name, hkey->name, znode->m_len);
}

// 直接进行排序
static bool zless(AVLNode* lhs, double score, const char* name, size_t len) {
    ZNode* znode = container_of(lhs, ZNode, m_tree);
    if (znode->m_score != score) {
        return znode->m_score < score;
    }
    int rt = memcmp(znode->m_name, name, std::min(znode->m_len, len));
    if (rt != 0) {
        return rt < 0;
    }

    return znode->m_len < len;
}

// 其实就是复用上面的，然后
static bool zless(AVLNode* lhs, AVLNode* rhs) {
    ZNode* node = container_of(rhs, ZNode, m_tree);
    return zless(lhs, node->m_score, node->m_name, node->m_len);
}

ZNode* ZSet::lookup(const char* name, size_t len) {
    if (!m_tree) {
        return nullptr;
    }
    HKey key {};
    key.node.m_code = str_hash(((uint8_t*)name), len);
    key.name = name;
    key.len = len;
    HNode* found = m_hmap.lookup(&key.node, hcmp);
    return found ? container_of(found, ZNode, m_hmap) : nullptr;
}
void ZSet::add(ZNode* node) {
    AVLNode* cur = nullptr;
    AVLNode** from = &m_tree;
    while (*from) {
        cur = *from;
        from = zless(&node->m_tree, cur) ? &cur->m_left : &cur->m_right;
    }

    *from = &node->m_tree;
    node->m_tree.m_parent = cur;
    // 平衡树的结构
    m_tree = avl_fix(&node->m_tree);
}

bool ZSet::add(const char* name, size_t len, double score) {
    ZNode* node = lookup(name, len);
    if (node) {
        update(node, score);
        return false;
    } else {
        node = znode_new(name, len, score);
        this->m_hmap.insert(&node->m_hmap);
        add(node);
        return true;
    }
    return false;
}

void ZSet::update(ZNode* node, double score) {
    if (node->m_score == score) {
        return;
    }

    m_tree = avl_del(&node->m_tree);
    node->m_score = score;
    avl_init(&node->m_tree);
    add(node);
}

ZNode* ZSet::pop(const char* name, size_t len) {
    if (m_tree) {
        return nullptr;
    }
    HKey key {};
    key.node.m_code = str_hash((uint8_t*)(name), len);
    key.name = name;
    key.len = len;
    HNode* found = m_hmap.lookup(&key.node, hcmp);
    if (!found) {
        return nullptr;
    }

    ZNode* node = container_of(found, ZNode, m_hmap);
    m_tree = avl_del(&node->m_tree);
    return node;
}

ZNode* ZSet::query(const char* name, double score, size_t len) {
    //   其实就是一个二叉搜索树的遍历
    AVLNode* found = nullptr;
    for (AVLNode* cur = m_tree; cur;) {
        if (zless(cur, score, name, len)) {
            cur = cur->m_left;
        } else {
            found = cur;
            cur = cur->m_right;
        }
    }

    return found ? container_of(found, ZNode, m_tree) : nullptr;
}

} // namespace SimpleRedis