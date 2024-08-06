#pragma once

#include <ostream>
#include <string>
#include <cstdint>
#include <fcntl.h>
#include <format>

namespace SimpleRedis {

// 下面是相关命令
enum class CmdError {
    ERR_UNKNOWN = 1,
    ERR_2BIG = 2,
    ERR_TYPE = 3,
    ERR_ARG = 4,
};

inline std::ostream& operator<<(std::ostream& os, const CmdError& cmd_error) {
    switch (cmd_error) {
    case CmdError::ERR_UNKNOWN: {
        os << "ERR_UNKNOWN";
        break;
    }
    case CmdError::ERR_2BIG: {
        os << "ERR_2BIG";
        break;
        ;
    }
    case CmdError::ERR_TYPE: {
        os << "ERROR_TYPE";
        break;
    }
    case CmdError::ERR_ARG: {
        os << "ERROR_ARG";
        break;
    }
    }
    return os;
}

enum class ConnectState {
    STATE_REQ = 0,
    STATE_RES = 1,
    STATE_END = 2,
};

inline std::ostream& operator<<(std::ostream& os, const ConnectState& connect_state) {
    switch (connect_state) {
    case ConnectState::STATE_REQ: {
        os << "STATE_REQ";
        break;
    }
    case ConnectState::STATE_RES: {
        os << "STATE_REG";
    }
    case ConnectState::STATE_END: {
        os << "STATE_END";
        break;
    }
    }
}

// 数据类型
enum class DataType {
    DATA_NULL = 0,   // NULL,
    DATA_ERR = 1,    // error
    DATA_STR = 2,    // std::string
    DATA_INT = 3,    // int64
    DATA_DOUBLE = 4, // double
    DATA_ARR = 5,    // Array
};

inline std::ostream& operator<<(std::ostream& os, const DataType& data_type) {
    switch (data_type) {
    case DataType::DATA_NULL: {
        os << "DATA_NULL";
        break;
    }
    case DataType::DATA_ERR: {
        os << "DATA_ERROR";
        break;
    }
    case DataType::DATA_STR: {
        os << "DATA_STR";
        break;
    }
    case DataType::DATA_INT: {
        os << "DATA_INT";
        break;
    }
    case DataType::DATA_DOUBLE: {
        os << "DATA_DOUBLE";
        break;
    }
    case DataType::DATA_ARR: {
        os << "DATA_ARR";
        break;
    }
    }
}
/*
这个是错误信息的Log

 */
// 之后补充

/**
 * @brief
 * 因为这个存储的是当前当前数据，所以要得到他真正的数值，因为还有指针
 *
 */
#define container_of(ptr, type, member)                                                  \
    ({                                                                                   \
        const typeof(((type*)0)->member)* __mptr = (ptr);                                \
        (type*)((char*)__mptr - offsetof(type, member));                                 \
    })

// 简单hash一手,然后直接溢出,
inline uint64_t string_hash(const std::string& str) {
    uint32_t h = 0x811C9DC5;
    for (const auto c : str) {
        h = (h + (uint8_t)c) * 0x01000193;
    }
    return h;
}

// 设置非堵塞
inline void fd_set_not_block(int fd) {
    int is_block = fcntl(fd, F_GETFL, 0);
    if (is_block & O_NONBLOCK) {
        return;
    }
    // 对这个文件描述符进行设置，设置成非堵塞,
    fcntl(fd, F_SETFL, is_block | O_NONBLOCK);
}

// 好牛
template <typename T>
std::ostream&
operator<<(typename std::enable_if<std::is_enum<T>::value, std::ostream>::type& os,
           const T& e) {
    return os << "\033[93m" << typeid(e).name() << "\033[0m::\033[36m"
              << static_cast<typename std::underlying_type<T>::type>(e) << "\033[0m";
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::optional<T>& op) {
    if (op.has_value()) {
        os << op.value();
    } else {
        os << "nil";
    }
    return os;
}

}; // namespace SimpleRedis