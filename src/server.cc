#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <fmt/core.h>
#include "include/tool.h"

static void do_something(int confd) {
    char read_buffer[64] = {};
    ssize_t n = read(confd, read_buffer, sizeof(read_buffer) - 1);
    if (n < 0) {
        error_message(fmt::format("read() error, error = {}, strerror = {}", errno,
                                  strerror(errno)));
    }

    printf("client says:  %s", read_buffer);

    char write_buffer[] = "world";
    write(confd, write_buffer, strlen(write_buffer));
}

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        error_message(fmt::format("socket error, errno = {}, strerror = {}", errno,
                                  strerror(errno)));
    }

    int val { 1 };
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    struct sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0); // 0.0.0.0
    int rt = bind(fd, (const struct sockaddr*)&addr, sizeof(addr));
    if (rt) {
        error_message(
            fmt::format("bind() error, err ={}, strerr = {}", errno, strerror(errno)));
    }

    rt = listen(fd, SOMAXCONN);

    while (true) {
        struct sockaddr_in client_addr {};
        socklen_t sock_len = sizeof(client_addr);
        int confd = accept(fd, (struct sockaddr*)&client_addr, &sock_len);
        if (confd < 0) {
            continue;
        }

        do_something(confd);
        close(confd);
    }
    close(fd);
    return 0;
}