/**
 * @file client.h
 * @author lzc (liuzechen.coder@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-08-05
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include "file/buffer.h"
#include "tools/common.h"
#include "file/file.h"
#include <arpa/inet.h>
#include <vector>

namespace SimpleRedis {
class Client {
public:
    Client() {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) {
            std::cout << "寄了" << std::endl;
        }
        sockaddr_in addr {};
        // 协议簇
        addr.sin_family = AF_INET;
        // 端口
        addr.sin_port = ntohs(1111);
        // 本地端口
        addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);
        int rv =
            connect(fd, reinterpret_cast<const struct sockaddr*>(&addr), sizeof(addr));
        if (rv) {
            std::cout << "寄了" << std::endl;
        }
        m_fd = fd;
    }

    // 往buffer 按照序列化的方式发送数据
    // 按照一定的格式去实现这个序列化
    void send(const std::vector<std::string>& cmds) {
        Buffer buffer {};
        uint32_t len { 4 };
        for (const std::string& cmd : cmds) {
            len += 4 + cmd.size();
        }

        buffer.append_number(len, 4);
        uint32_t n = cmds.size();
        buffer.append_number(n, 4);
        for (const auto& str : cmds) {
            auto p = (uint32_t)str.size();
            buffer.append_number(p, 4);
            buffer.append_string(str);
        }
        auto ok = m_fd.write_buffer_b(buffer);
        assert(ok);
    }

    void on_reponse(Buffer& buffer, const std::string& pre = "") {
        auto type = static_cast<DataType>(buffer.get_number<int>(1));
        uint32_t str_len {};
        uint32_t arr_len {};
        std::string_view str;
        std::string_view msg {};
        int64_t flag {};
        CmdError res_code {}; // 错误码
        double value {};
        // 卧槽，点击一下就出来了
        switch (type) {

        case DataType::DATA_NULL: {
            std::cout << pre << "null" << std::endl;
            break;
        }
        case DataType::DATA_ERR: {
            res_code = static_cast<CmdError>(buffer.get_number<uint32_t>(4));
            str_len = buffer.get_number<uint32_t>(4);
            msg = buffer.get_string_view(str_len);
            // 这里为什么不能调用 fmt ?
            std::cout << res_code << ' ' << str_len << ' ' << msg << ' ';
            break;
        }
        case DataType::DATA_STR: {
            str_len = buffer.get_number<uint32_t>(4);
            str = buffer.get_string_view(str_len);
            std::cout << pre << "[str]:" << ' ' << str << ' ';
            break;
        }
        case DataType::DATA_INT: {
            flag = buffer.get_number<int64_t>(8);
            std::cout << pre << "[int: ]" << flag << std::endl;
            break;
        }
        case DataType::DATA_DOUBLE: {
            value = buffer.get_number<double>(8);
            std::cout << pre << "double:" << value << " ";
            break;
        }
        case DataType::DATA_ARR: {
            arr_len = buffer.get_number<uint32_t>(4);
            std::cout << pre << "[arr: len]= " << arr_len << std::endl;
            for (size_t i = 0; i < arr_len; i++) {
                on_reponse(buffer, pre + "");
            }
            break;
        }
        default: {
            std::cout << "寄了" << std::endl;
        }
        }
    }

private:
    File m_fd {};
};
}; // namespace SimpleRedis