#pragma once

#include "file/buffer.h"
#include "file/file.h"
#include "tools/common.h"
#include "data_structure/list.h"
#include "sys/poll.h"

namespace SimpleRedis {
class TcpConnect {
public:
    explicit TcpConnect(File&& f, ConnectState conn_state)
        : m_file(std::move(f)), m_state { conn_state }, m_read_buffer {},
          m_write_buffer {}, idle_start { get_monotonic_usec() }, idle_node {} {
        m_file.set_no_block();
    }
    int get_fd() const noexcept { return m_file.get_fd(); }

    short int get_event() const {
        if (m_state == ConnectState::STATE_REQ) {
        }
    }

private:
    File m_file {};
    ConnectState m_state {}; // 连接状态

    Buffer m_read_buffer {};
    Buffer m_write_buffer {};

    uint64_t idle_start {}; // 从xxxx开始

    List idle_node {};
};
}; // namespace SimpleRedis