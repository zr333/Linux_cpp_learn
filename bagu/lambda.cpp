#include <iostream>

/*
    什么是函数对象？
    - 函数对象（Function Object），也叫函数仿函数（Functor），是一种可以像函数一样被调用的对象。它的本质是定义了
operator()
    - 运算符的类或结构体的实例。

    lambda表达式本质：函数对象。lambda表达式本质上是一个匿名的、可以被调用的函数对象，它会被编译器转化为一个结构体，其中包含：
    捕获列表：指定 lambda 如何捕获外部变量。
    operator()：这个函数体即 lambda 的主体，它实现了函数调用的逻辑。

    例如：
    auto lambda = [a, b]() {
    std::cout << a << " " << b << std::endl;
    会被转化为：
    struct Lambda {
    int a;
    int b;
    void operator()() {
        std::cout << a << " " << b << std::endl;
    }
};

};

*/

int g = 100; // 全局变量

class Add {
public:
    int operator()(int a, int b) { return a + b; }
};

int main() {
    Add add;
    int a = 1, b = 2;

    std::cout << "普通仿函数：" << std::endl;
    std::cout << add.operator()(a, b) << std::endl;
    std::cout << add(a, b) << std::endl;
    
    std::cout << "lambde:" << std::endl;
    auto a1 = [a, b]() -> int { return a + b; };
    std::cout << a1() << std::endl;

    std::cout << "------------------------------------------------------" << std::endl;

    // lambda表达式是一种创建函数对象的方式，也成为匿名函数
    /*
    [capture](parameter_list) -> return_type {
        function_body
    };
    - capture（捕获列表）：告诉 lambda 需要从外部作用域里带哪些变量进来。
    - parameter_list：参数列表，和普通函数一样。
    - return_type（可选）：返回值类型。C++14 以后一般可以省略，编译器会自动推断。
    - function_body：函数体。
    */

    // 值捕获
    int x = 10, y = 20;
    auto f = [x]() { return x; };
    std::cout << f() << std::endl;
    /*
    采用值捕获时，lambda函数生成的类用捕获变量的值初始化自己的成员变量;
    */

    // 引用捕获
    auto f1 = [&x]() { 
        x += 3;
        return x; 
        };  // 在lambda函数的外部，变量的值不会被修改。
    std::cout << f1() << std::endl;
    std::cout << x << std::endl;
    /*
    当使用引用传递捕获外部变量时，lambda 并没有复制外部变量，而是将外部变量的引用（即指针）保存在 lambda
    内部。这意味着，lambda 在调用时会直接操作外部变量的内存。
    */

    // 捕获所有
    auto f2 = [=]() { return x + y; };
    auto f3 = [&]() { return ++x + ++y; };
    std::cout << f2() << std::endl;
    std::cout << f3() << std::endl;


    // lambda可以直接访问静态局部变量和全局变量
    static int sg = 101;
    auto a2 = []() { std::cout << g << std::endl; };
    a2();
    auto a3 = []() { std::cout << sg << std::endl; };
    a3();

    return 0;
}