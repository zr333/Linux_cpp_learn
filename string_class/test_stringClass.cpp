#include "stringClass.cpp"
#include <cassert>
#include <iostream>

void test_default_constructor() {
    myString s;
    assert(s.size() == 0);
    assert(std::string(s.c_str()) == "");
    std::cout << "✅ 默认构造测试通过" << std::endl;
}

void test_cstr_constructor() {
    myString s("Hello");
    assert(s.size() == 5);
    assert(std::string(s.c_str()) == "Hello");
    std::cout << "✅ C字符串构造测试通过" << std::endl;
}

void test_copy_constructor() {
    myString s1("World");
    myString s2(s1);
    assert(s2.size() == 5);
    assert(std::string(s2.c_str()) == "World");
    std::cout << "✅ 拷贝构造测试通过" << std::endl;
}

void test_move_constructor() {
    myString s1("Move");
    myString s2(std::move(s1));
    assert(s2.size() == 4);
    assert(std::string(s2.c_str()) == "Move");
    assert(s1.size() == 0); // 移动后原对象应为空
    std::cout << "✅ 移动构造测试通过" << std::endl;
}

void test_copy_assignment() {
    myString s1("Copy");
    myString s2;
    s2 = s1;
    assert(s2.size() == 4);
    assert(std::string(s2.c_str()) == "Copy");
    std::cout << "✅ 拷贝赋值测试通过" << std::endl;
}

void test_move_assignment() {
    myString s1("MoveAssign");
    myString s2;
    s2 = std::move(s1);
    assert(s2.size() == 10);
    assert(std::string(s2.c_str()) == "MoveAssign");
    assert(s1.size() == 0); // 移动后原对象应为空
    std::cout << "✅ 移动赋值测试通过" << std::endl;
}

void test_concatenation() {
    myString s1("Hello, ");
    myString s2("World!");
    s1 += s2;
    assert(s1.size() == 13);
    assert(std::string(s1.c_str()) == "Hello, World!");
    std::cout << "✅ 字符串拼接测试通过" << std::endl;
}

int main() {
    test_default_constructor();
    test_cstr_constructor();
    test_copy_constructor();
    test_move_constructor();
    test_copy_assignment();
    test_move_assignment();
    test_concatenation();
    
    std::cout << "\n🎉 所有测试通过！" << std::endl;
    return 0;
}