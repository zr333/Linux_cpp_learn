#include <stdio.h>
#include <string.h>

int main() {
    char *p = NULL;            // 野指针
    *p = "hello world";

    //int arr[5];
    //arr[10] = 20;  // 越界写入 → 访问非法内存

    return 0;
}
