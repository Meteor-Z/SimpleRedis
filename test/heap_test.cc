#include "data_structure/heap.h"
#include <cstddef>
#include <memory>
#include <vector>

int main() {
    std::vector<size_t> arr(1e5 + 10, 0);

    std::unique_ptr<SimpleRedis::Heap> heap_test_1 =
        std::make_unique<SimpleRedis::Heap>();

    for (int i = 0; i < arr.size(); i++) {
        arr[i] = i;
        heap_test_1->push(i, &arr[i]);
    }
    heap_test_1->check();

    // ---------------------------------------------

    std::unique_ptr<SimpleRedis::Heap> heap_test_2 =
        std::make_unique<SimpleRedis::Heap>();
    std::vector<size_t> arr_2(100, 0);
    for (size_t i = 0; i < arr.size(); i++) {
        arr[i] = i;
        heap_test_2->push(i, &arr[i]);
    }
    heap_test_2->check();

    // 加入新的测试用例
}