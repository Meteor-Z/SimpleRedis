#include "tools/thread_pool.h"
#include <iostream>

int function(int a, int b) {
    std::cout << a + b << std::endl;
    return a + b;
}
int main() {
    ThreadPool thread_pool(4);
    std::vector<std::future<int>> rets;
    std::function<int(int, int)> f = [](int a, int b) { return a + b; };

    for (int i = 0; i < 100; i++) {
        auto ret = thread_pool.submit(function, i, i + 1);
        rets.push_back(std::move(ret));
    }
  
    for (int i = 0; i < 100; i++) {
        std::cout << rets[i].get() << std::endl;
    }
}