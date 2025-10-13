#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "threadPool.h"

#define MAX_EVENTS 1024

int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void working(int cfd, int epoll_fd) {
    char buf[1024] = {0};
    while (1) {
        memset(buf, 0, sizeof(buf));
        int len = read(cfd, buf, sizeof(buf));
        if (len > 0) {
            printf("client say: %s\n", buf);

            write(cfd, buf, len);
        } else if (len == -1 && errno == EAGAIN) {
            // 非阻塞模式下没有数据可读
            break;
        } else if (len == 0) {
            printf("client closed...\n");
            
            break;
        } else {
            perror("read");
            break;
        }
    }
    
    close(cfd);
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, cfd, NULL);
}

int main() {
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("socket");
        return -1;
    }

    // 端口复用
    int optval = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(9999);
    saddr.sin_addr.s_addr = INADDR_ANY;
    int ret = bind(lfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1) {
        perror("bind");
        return -1;
    }

    ret = listen(lfd, 128);
    if (ret == -1) {
        perror("listen");
        return -1;
    }

    ThreadPool pool(4);

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        return -1;
    }

    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = lfd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, lfd, &ev);

    // 主线程不断的等待连接
    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(-1);
        } else if (nfds == 0) {
            printf("超时...\n");
            continue;
        } else {
            for (int i = 0; i < nfds; i++) {
                int fd = events[i].data.fd;
                if (fd == lfd) {
                    // 有新的客户端连接
                    struct sockaddr_in caddr;
                    socklen_t len = sizeof(caddr);
                    int cfd = accept(lfd, (struct sockaddr *)&caddr, &len);
                    if (cfd == -1) {
                        perror("accept");
                        continue;
                    }
                    // 输出客户端信息
                    // 把网络字节序的二进制 IP 地址转换为可读的点分十进制字符串（IPv4）
                    char ip[16] = {0};
                    inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip));
                    unsigned short client_port = ntohs(caddr.sin_port);
                    printf("client ip: %s, port: %d\n", ip, client_port);

                    // 设置文件描述符为非阻塞模式
                    if (set_nonblocking(cfd) == -1) {
                        perror("set_nonblocking");
                        close(cfd);
                        continue;
                    }
                    // 将新连接的文件描述符添加到epoll中
                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = cfd;
                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, cfd, &ev);
                } else {
                    int cfd = events[i].data.fd;
                    pool.addTask([cfd, epoll_fd]() { working(cfd, epoll_fd); });
                }
            }
        }
    }

    close(lfd);
    close(epoll_fd);
    return 0;
}