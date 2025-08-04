#include <iostream>

void func1()
{
    // 简单的内存泄漏：new 分配的内存没有 delete
    int *leak = new int;
}

void func2()
{
    //越界
    int *overflow = new int[3];
    overflow[3] = 123;  // 越界写入
    //错误释放
    delete overflow;
    
    //使用未初始化的内存
    int *uninit;
    int num = *uninit;

    //重复释放
    int *dfree = new int[2];
    delete[] dfree;
    delete[] dfree;  
    
    // 使用被回收的空间
    int *p4 = new int;
    delete p4;
    *p4 = 3;

}
int main()
{
    func1();
    func2();
    return 0;
}
