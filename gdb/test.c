#include <stdio.h>
#include <stdlib.h>


void func3() {
    // 创建更容易触发段错误的情况
    int *null_ptr = NULL;
    *null_ptr = 10;  // 访问空指针，几乎肯定会触发段错误
}

int main() {
    func3();  // 调用这个函数来触发段错误
    return 0;
}