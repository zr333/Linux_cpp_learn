#include <iostream>
#include <memory>

class B;  // 前向声明

class A {
public:
    std::shared_ptr<B> b_ptr;  // 使用 weak_ptr 避免循环引用

    A() { std::cout << "A 构造!" << std::endl; }
    ~A() { std::cout << "A 析构!" << std::endl; }
};

class B {
public:
    std::weak_ptr<A> a_ptr;


    B() { std::cout << "B 构造!" << std::endl; }
    ~B() { std::cout << "B 析构!" << std::endl; }
};

int main() {
    /*
    make_shared 和 裸指针创建的区别：
    好处：在使用 new 的方式申请的时候，先用 new
    申请指向对象的内存，再申请shared_ptr中维护计数部分的内存，所以是两次申请。
    用make_shared的方式，是把指向对象部分和维护计数部分合在一起，一起申请，所以是一次申请。
    问题：由于弱引用计数的存在，make_shared创建的智能指针引用的对象，可能无法得到及时的释放，只有当强/弱引用都为0时，才能释放make_shared申请的一整块内存。
    2
    */
    std::shared_ptr<A> a = std::make_shared<A>();  // 使用 std::make_shared<T>() 创建一个对象时，这个对象会被分配到堆上
    std::shared_ptr<B> b = std::make_shared<B>();

    a->b_ptr = b;  
    b->a_ptr = a;  

    return 0;
}
