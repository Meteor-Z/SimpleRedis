#include <cerrno>
#include <cstdio>
#include <cstring>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fmt/core.h>
#include "fmt/base.h"
#include "fmt/format.h"
#include "include/tool.h"

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        fmt::println("socket() error, error = {}, strerror = {}", errno, strerror(errno));
    }

    sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1

    int rt = connect(fd, (const struct sockaddr*)(&addr), sizeof(addr));

    if (rt) {
        fmt::println("connect error, errnp = {}, str = {}", errno, strerror(errno));
    }

    char msg[] = "hello";
    char read_buffer[64] {};
    ssize_t n = read(fd, read_buffer, sizeof(read_buffer) - 1);
    if (n < 0) {
        error_message(fmt::format("read() error ,errno = {}, strerro = {}", errno,
                                  strerror(errno)));
    }

    fmt::println("server says: {}", read_buffer);
    close(fd);
    return 0;
}