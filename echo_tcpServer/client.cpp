#include<arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(){

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        perror("socket error");
        exit(1);
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9999);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ret = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(ret < 0){
        perror("connect error");
        exit(1);
    }

    char writeBuf[1024];
    char readBuf[1024];
    while(1){
        memset(writeBuf, 0, sizeof(writeBuf));
        fgets(writeBuf, sizeof(writeBuf), stdin);
        write(sockfd, writeBuf, strlen(writeBuf));

        memset(readBuf, 0, sizeof(readBuf));
        int len = read(sockfd, readBuf, sizeof(readBuf));
        if (len > 0) {
            printf("Server response: %s\n", readBuf);
        } else if (len == 0) {
            // 表示服务端断开连接 printf("server closed...\n");
            break;
        }else {
            perror("read error");
            break;
        }
    }


    close(sockfd);
    return 0;
}