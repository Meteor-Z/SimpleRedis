#include "avl.h"
#include "hash_table.h"


namespace SimpleRedis {
struct ZNode {
    public:
    private:
    AvlNode avl_node {};
    HNode h_node{};
};

class ZSet {};

} // namespace SimpleRedis