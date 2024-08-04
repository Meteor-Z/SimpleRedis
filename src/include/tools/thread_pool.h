/**
 * @file thread_pool.h
 * @author lzc (liuzechen@coder.com)
 * @brief 线程池
 * @version 0.1
 * @date 2024-08-04
 *
 * @copyright Copyright (c) 2024
 *
 */

/*
- 线程池的队列，因为要实现安全队列，所以可以简单封装一下（STL线程完全不安全）
- std::future是不可copy的，所以说只能 std::move
- 虽然说函数有std::future<int> 这样的返回值，但是可以使用
lambda函数将其再进行包装一层，这样返回值就是std::function<void()> 了

*/
#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <queue>
#include <mutex>
#include <ranges>
#include <thread>
#include <utility>
#include <vector>

template <typename T>
struct SafeQueue {
public:
    SafeQueue() = default;
    SafeQueue(const SafeQueue&) = default;
    SafeQueue(SafeQueue&&) = delete;
    SafeQueue& operator=(const SafeQueue&) = delete;
    SafeQueue& operator=(SafeQueue&&) = delete;
    ~SafeQueue() = default;

public:
    void push(const T& value) {
        std::lock_guard<std::mutex> lock { mutex };
        queue.push(value);
    }

    size_t size() {
        std::lock_guard<std::mutex> lock { mutex };
        return queue.size();
    }

    bool empty() {
        std::lock_guard<std::mutex> lock { mutex };
        return queue.empty();
    }

    bool front(T& value) {
        std::lock_guard<std::mutex> lock { mutex };
        if (queue.empty()) {
            return false;
        }
        value = std::move(queue.front());
        queue.pop();
        return true;
    }

private:
    std::queue<T> queue {};
    std::mutex mutex {};
};

struct ThreadPool {
public:
    using Task = std::function<void()>;

public:
    ThreadPool() = default;
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;
    ~ThreadPool() { stop(); }
    ThreadPool(size_t threads_size) : is_running(true) {
        threads.reserve(threads_size);
        while (threads_size--) {
            threads.emplace_back(&ThreadPool::loop, this);
        }
    }

    template <typename Fun, typename... Args>
    auto submit(Fun&& func, Args... args) -> std::future<decltype(func(args...))> {
        auto pt = std::make_shared<std::packaged_task<decltype(func(args...))()>>(
            std::bind(std::forward<Fun>(func), std::forward<Args>(args)...));

        Task wrpper_func = [pt]() { (*pt)(); };
        
        queue.push(wrpper_func);
        condtion.notify_one();
        return pt->get_future();
    }

    void stop() {
        if (!is_running) {
            return;
        }
        is_running = false;
        condtion.notify_all();

        for (auto& item : threads) {
            if (item.joinable()) {
                item.join();
            }
        }
    }

private:
    void loop() {
        while (true) {
            Task task {};
            {
                std::unique_lock<std::mutex> lock { mutex };
                if (queue.empty()) {
                    condtion.wait(lock);
                }

                if (!is_running) {
                    return;
                }

                queue.front(task);
                condtion.notify_one();
            }

            if (task) {
                task();
            }
        }
    }

private:
    SafeQueue<Task> queue {};
    std::vector<std::thread> threads {};
    std::mutex mutex {};
    std::condition_variable condtion {};
    bool is_running { false };
};