#include "avl.h"
#include "hash_table.h"
#include "tools/common.h"
#include <format>
#include <string>
#include <unistd.h>

namespace SimpleRedis {
struct ZNode {
public:
    ZNode() = delete;
    ZNode(const std::string& name_, double score_)
        : avl_node {}, h_node(string_hash(name_)), score(score_) {}
    friend std::ostream& operator<<(std::ostream& os, const ZNode& node) {
        os << std::format("ZNode: name = {}, score = {}", node.name, node.score);
        return os;
    }
    AvlNode avl_node {}; // (score, name)
    HNode h_node {};     // by name
    double score = 0;
    size_t len = 0;
    std::string name {};
};

struct ZSet {
    AvlNode* tree { nullptr };
    HMap hmap;
};

inline bool zless(AvlNode* lhs, double score, const std::string& name) {

    ZNode* zl = container_of(lhs, ZNode, avl_node);
    if (zl->score != score) {
        return zl->score < score;
    }
    return zl->name < name;
}

inline bool zless(AvlNode* lhs, AvlNode* rhs) {
    ZNode* zr = container_of(rhs, ZNode, avl_node);
    return zless(lhs, zr->score, zr->name);
}

struct HKey { // 辅助用, 因为想进入hash table必须有有一个包含HNode的结构体
    HNode node;
    std::string name;
    HKey() = delete;
    HKey(const std::string& name_) : node { string_hash(name_) }, name { name_ } {}
};

inline Cmp hcmp = [](HNode* node, HNode* key) -> bool {
    if (node->hcode != key->hcode) {
        return false;
    }
    ZNode* znode = container_of(node, ZNode, h_node);
    HKey* hkey = container_of(key, HKey, node);
    return znode->name == hkey->name;
};

class ZNodeCollection {
public:
    class iterator {
    public:
        iterator(ZNode* ptr_, int64_t step_) : ptr(ptr_), step(step_) {}
        iterator&  operator++() {
            ptr = container_of(, type, member)
        }

    private:
        ZNode* ptr {};
        int64_t step {};
    };

private:
    ZNode* data {};
    int64_t length {};
};

} // namespace SimpleRedis