#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
int main() {
    // 模拟mmap和wirte实现的零拷贝技术，1.txt为磁盘文件，2.txt为网卡
    // 将磁盘文件读取发送到网卡
    int fd1 = open("1.txt", O_RDWR);
    int fd2 = open("2.txt", O_RDWR);


    //int len = lseek(fd1, 0, SEEK_END); // 返回的是 文件开头到文件末尾的字节数，也就是 文件大小。
    struct stat st;
    fstat(fd1, &st);
    int len = st.st_size;
    printf("%d\n", len);

    void *shared_addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);

    // printf("%s\n", shared_addr);
    // write(fd1, "acd", 3);
    // printf("%s\n", shared_addr);

    write(fd2, shared_addr, len);
    munmap(shared_addr, len);

    close(fd1);
    close(fd2);
    return 0;
}