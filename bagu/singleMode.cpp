#include <iostream>

/*
单例模式的特点：
构造函数和析构函数为私有类型，目的是禁止外部构造和析构。
拷贝构造函数和赋值构造函数是私有类型，目的是禁止外部拷贝和赋值，确保实例的唯一性。
类中有一个获取实例的静态方法，可以全局访问。
*/

// 内部静态变量的懒汉实现
class Single {
private:
    // 构造函数私有化
    Single() { std::cout << "构造函数" << std::endl; }
    ~Single() { std::cout << "析构函数" << std::endl; }

public:
    // 获取单例实例接口
    static Single &getInstance() {
        /**
         * 局部静态特性的方式实现单实例。
         * 静态局部变量只在当前函数内有效，其他函数无法访问。
         * 静态局部变量只在第一次被调用的时候初始化，也存储在静态存储区，生命周期从第一次被初始化起至程序结束止。
         */
        static Single instance;
        return instance;
    }
    // 禁止拷贝和赋值操作
    Single(const Single &) = delete;
    Single &operator=(const Single &) = delete;
};


void testSingle() {
    // 获取单例对象
    Single &instance1 = Single::getInstance();
    Single &instance2 = Single::getInstance();

    // 验证两个引用是否指向同一个实例
    if (&instance1 == &instance2) {
        std::cout << "instance1 and instance2 are the same instance." << std::endl;
    } else {
        std::cout << "Error: instance1 and instance2 are different instances!" << std::endl;
    }

    // 尝试拷贝构造（应编译失败）
    //Single instance3 = instance1; // 如果取消注释，会导致编译错误

    // 尝试赋值操作（应编译失败）
    // Single instance4;
    //instance4 = instance1; // 如果取消注释，会导致编译错误
}

int main() {
    testSingle();
    return 0;
}
