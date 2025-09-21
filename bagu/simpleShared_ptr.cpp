#include <iostream>

template <typename T> class myShared_ptr {
  private:
    T *ptr;
    int *count;

  public:
    myShared_ptr(T *ptr) : ptr(ptr), count(new int(1)) {}

    ~myShared_ptr() { release(); }

    // 拷贝构造
    myShared_ptr(const myShared_ptr<T> &other)
        : ptr(other.ptr), count(other.count) {
        if (count) {
            ++(*count);
        }
    }
    // 赋值
    myShared_ptr& operator=(const myShared_ptr<T> &other) {
        if (this != other) {
            release();

            ptr = other.ptr;
            count = other.count;
            (*count)++;
        }
        return *this;
    }

    void release() {
        (*count)--;
        if ((*count) == 0) {
            delete ptr;
            delete count;
        }
    }
    T& operator*() const {
        return *ptr;
    }

    T* operator->() const {
        return ptr;
    }
    T* get() const {
        return ptr;
    }
    size_t use_count() const {
        return count ? *count : 0; 
    }

};


// 测试
class MyClass
{
private:
    int value;
public:
    
    MyClass(int arg):value(arg) { std::cout << "MyClass 构造函数\n"; }
    ~MyClass() { std::cout << "MyClass 析构函数\n"; }
    void do_something() { std::cout << "MyClass::do_something() 被调用\n"; }
};
int main(){

    {
        myShared_ptr<MyClass> ptr1(new MyClass(10));
        {
            myShared_ptr<MyClass> ptr2(ptr1);
            myShared_ptr<MyClass> ptr3 = ptr2;
            ptr1->do_something();
            ptr2->do_something();
            ptr3->do_something();
            std::cout << "引用计数: " << ptr1.use_count() << std::endl; // 2
        }
        std::cout << "引用计数: " << ptr1.use_count() << std::endl; // 1
    }

   

  return 0;
}