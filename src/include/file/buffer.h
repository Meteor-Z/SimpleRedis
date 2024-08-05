#pragma once

#include <cstddef> // std::byte
#include <cstring>
#include <iostream>
#include <pthread.h>
#include <string_view>
#include <vector>
#include <cassert>

namespace SimpleRedis {

class File;

class Buffer {
public:
    Buffer() = default;

    std::size_t size() const { return data.size(); }
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) = delete;
    ~Buffer() = default;

    void reset() { pos = 0; }
    void clear() {
        data.clear();
        reset();
    }
    bool is_read_end() const { return pos == data.size(); }

    friend std::ostream& operator<<(std::ostream& os, const Buffer& bytes) {
        std::cout << std::hex;
        for (int i = 0; i < bytes.data.size(); i++)
            os << (i == 0 ? "[" : "") << "0x" << std::to_integer<int>(bytes.data[i])
               << (i + 1 == bytes.data.size() ? "]" : ", ");
        std::cout << std::dec;
        os << " len = " << bytes.size();
        return os;
    }

    void append_string(const std::string& str) {
        const size_t size = str.size();
        const size_t data_size = data.size();
        data.resize(data_size + size);
        std::memcpy(data.data() + data_size, str.data(), size);
    }
    void append_stringview(const std::string_view& str_view) {
        const size_t size = str_view.size();
        const size_t data_size = data.size();
        data.resize(data_size + size);
        std::memcpy(data.data() + data_size, str_view.data(), size);
    }

    // 坏了，完全不懂了
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    void append_number(const T& num, int N) {
        const size_t data_size = data.size();
        data.resize(data_size + N);
        std::memcpy(data.data() + data_size, &num, N);
    }

    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    void inset_number(const T& num, std::size_t index, int N) {
        assert(0 <= index && index < data.size());
        if (index + N > data.size())
            data.resize(index + N);
        std::memcpy(data.data() + index, &num, N);
    }

    void append_byte_move(Buffer&& other) {
        if (this == &other) {
            return;
        }
        data.insert(data.end(), std::make_move_iterator(other.data.begin()),
                    std::make_move_iterator(other.data.end()));
        other.data.clear();
    }

    std::string_view get_string_view(int len) {
        const size_t data_size = data.size();
        const size_t read_size = std::min(data_size - pos, static_cast<std::size_t>(len));
        std::string_view view =
            std::string_view(reinterpret_cast<const char*>(data.data() + pos), read_size);
        pos += read_size;
        return view;
    }

    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    T get_number(int N) {
        T num = 0;
        std::memcpy(&num, data.data() + pos, N);
        pos += N;
        return num;
    }

public:
    std::vector<std::byte> data {};
    std::size_t pos { 0 };
};

} // namespace SimpleRedis

// /**
//  * @brief 对字节进行封装
//  *
//  */

// #include <cstddef>
// #include <vector>
// #include <string.h>

// namespace SimpleRedis {
// class Buffer {
// public:
//     explicit Buffer(int size);

//     Buffer(const Buffer&) = delete;
//     Buffer(Buffer&&) = delete;
//     Buffer& operator=(const Buffer&) = delete;
//     Buffer& operator=(Buffer&&) = delete;

//     ~Buffer();

//     // 可以读的字节数
//     int can_read_bytes_num() const noexcept { return write_index - read_index; }

//     // 返回可写的字节树
//     int can_write_bytes_num() const noexcept { return buffer.size() - write_index; }

//     int get_read_index() const noexcept { return read_index; }

//     // 得到write的index坐标
//     int get_write_index() const noexcept;

//     void write_to_buffer(const char* buffer, int size) {
//         if (size > can_write_bytes_num()) {
//             int new_size = { static_cast<int>(1.5 * (write_index + size)) };
//             resize_buffer(new_size);
//         }

//         // 调整, 并且忽略这个目标
//         int result_ = memcmp(&buffer[write_index], buffer, size);
//         // 坐标也要更新e
//         write_index += size;
//     }

//     // 读出的字节
//     // 大小
//     void read_from_buffer(std::vector<char>& re, int size) {
//         if (can_read_bytes_num() == 0) {
//             return;
//         }

//         // 可以读入的字节
//         int read_size { std::min(size, can_read_bytes_num()) };

//         std::vector<char> temp(read_size);
//         memcpy(&temp[0], &buffer[read_index], read_size);
//         re.swap(temp);

//         // 可以读入的下标要往右边移动，移动的距离就是 read_size
//         read_index += read_size;
//         adjust_buffer();
//     }

//     // 调整，将buffer变成最初始的状态
//     // 也就是从头开始读取，移动一下。
//     void adjust_buffer() {
//         // 读的字节数 > 整个字节的 1/3, 才会进行调整
//         if (read_index < static_cast<int>(buffer.size() / 3)) {
//             return;
//         }

//         std::vector<char> new_buffer(buffer.size());

//         int count { can_read_bytes_num() };
//         memcpy(&new_buffer[0], &buffer[read_index], count);
//         buffer.swap(new_buffer);
//         read_index = 0;
//         write_index = read_index + count;

//         new_buffer.clear();
//     }

//     /**
//      * @brief 扩容
//      *
//      * @param new_size
//      */
//     void resize_buffer(int new_size) {
//         std::vector<char> new_buffer(new_size);

//         int count = std::min(new_size, can_read_bytes_num());

//         // 进行拷贝
//         memcpy(&buffer[0], &buffer[read_index], count);

//         buffer.swap(new_buffer);

//         read_index = 0;
//         write_index = read_index + count;
//     }

//     // 将read的index向右边移动 size 距离
//     void adjust_read_index(int size) {
//         size_t j = read_index + size;
//         if (j >= buffer.size()) {
//             // 这里是寄了
//             return;
//         }
//         read_index = j;
//         adjust_buffer();
//     }

//     // 往右边调整，将 write_indx 从新坐标开始
//     void adjust_write_index(int size) {
//         size_t j = write_index + size;
//         if (j >= buffer.size()) {
//             // 寄了
//             return;
//         }
//         write_index = j;
//         adjust_buffer();
//     }

//     std::vector<char>& get_buffer() noexcept { return buffer; }

// private:
//     int read_index { 0 };  ///< 读取的位置
//     int write_index { 0 }; // 写入的位置
//     int size { 0 };        ///< 大小

// public:
//     std::vector<char> buffer {}; // 缓冲区
// };
// }; // namespace SimpleRedis