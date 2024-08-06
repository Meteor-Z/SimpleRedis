#include "tools/common.h"
#include <endian.h>
#include <unistd.h>
#include <vector>
#include "buffer.h"

namespace SimpleRedis {
class File {
public:
    File() : fd(-1) {}
    explicit File(int fd_) : fd(fd_) {}
    File(const File&) = delete;
    File(File&& file) = delete;
    File& operator=(const File&) = delete;
    File& operator=(File&&) = delete;
    
    File& operator=(int fd_) {
        fd = fd_;
        return *this;
    }

    ~File() {
        // 关闭文件描述符
        close(fd);
    }

    int get_fd() const noexcept { return fd; }

    void set_no_block() const noexcept { fd_set_not_block(fd); }

    // 向对方输出
    bool write_buffer_b(Buffer& buffer, size_t count = 0) const {
        if (count == 0) {
            count = buffer.data.size() - buffer.pos;
        }
        ssize_t bytes_written = 0;
        while (bytes_written < count) {
            ssize_t rv = write(fd, buffer.data.data() + buffer.pos + bytes_written,
                               count - bytes_written);
            assert((size_t)rv <= count - bytes_written);

            if (rv <= 0) {
                return false;
            }
            // 那这里可不可能出现write(.., .., 0)嘛? 不会, 有while条件卡着
            bytes_written += (size_t)rv;
        }
        buffer.pos += static_cast<std::size_t>(bytes_written);
        return true;
    }

    bool read_buffer_b(Buffer& buffer, size_t count = 0) const {
        std::size_t end = buffer.data.size();
        buffer.data.resize(end + count);
        ssize_t bytes_read = 0;
        while (bytes_read < count) {
            ssize_t rv =
                read(fd, buffer.data.data() + end + bytes_read, count - bytes_read);
            if (rv <= 0) {
                buffer.data.resize(end + bytes_read);
                return false;
            }
            assert((size_t)rv <= count - bytes_read);
            bytes_read += rv;
        }
        return true;
    }

    int write_bytes_nb(Buffer& buffer) const {
        ssize_t rv = 0;
        do {
            ssize_t count = buffer.data.size() - buffer.pos;
            rv = write(fd, buffer.data.data() + buffer.pos, count);
        } while (rv < 0 && errno == EINTR);
        if (rv < 0 && errno == EAGAIN) {
            return 1; // got EAGAIN, stop.
        }
        if (rv < 0) {
            return -1;
        }
        buffer.pos += rv;
        assert(buffer.pos <= buffer.size());
        if (buffer.pos == buffer.size()) {
            return 0;
        }
        return -1;
    }

    int read_B_bytes_nb(Buffer& buffer, size_t count) const {
        std::size_t end = buffer.data.size();
        buffer.data.resize(end + count);
        ssize_t rv = 0;
        do {
            rv = read(fd, buffer.data.data() + end, count);
        } while (rv < 0 && errno == EINTR);

        if (rv < 0 && errno == EAGAIN) {
            buffer.data.resize(end);
            return 1;
        }

        if (rv < 0) {
            return -1;
            buffer.data.resize(end);
        }

        if (rv == 0) {
            buffer.data.resize(end);
            return 2;
        }
        return 0;
    }

private:
    int fd {}; // 文件描述符
};
}; // namespace SimpleRedis