#include <fcntl.h>
#include <stdio.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
int main() {
    int fd1 = open("1.txt", O_RDWR);
    int fd2 = open("2.txt", O_RDWR);

    // int len = lseek(fd1, 0, SEEK_END);
    // lseek(fd1, 0, SEEK_SET);// 将文件偏移挪到起始地址
    struct stat st;
    fstat(fd1, &st);
    int len = st.st_size;

    int send_len = sendfile(fd2, fd1, 0, len);
    printf("%d\n", send_len);
    close(fd1);
    close(fd2);
    return 0;
}