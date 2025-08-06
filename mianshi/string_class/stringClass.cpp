// 实现 简易string类
#include <cstring>
#include <iostream>
using namespace std;

class myString {
public:
    // 1.默认构造
    myString() : m_data(new char[1]{'\0'}), m_size(0) {}

    // 2.从C字符串构造
    myString(const char *str)
    {
        if (str)
        {
            m_size = strlen(str);
            m_data = new char[m_size + 1];
            strcpy(m_data, str);
        }
        else
        {
            // 处理传入 nullptr 的情况
            m_data = new char[1];
            m_data[0] = '\0';
            m_size = 0;
        }
    }

    // 3. 拷贝构造（深拷贝）
    myString(const myString &other)
        : m_data(new char[other.m_size + 1]), m_size(other.m_size)
    {
        strcpy(m_data, other.m_data);
    }

    // 4. 移动构造
    myString(myString &&other) noexcept
        : m_data(other.m_data), m_size(other.m_size) // 相当于浅拷贝
    {
        // 移动构造后，other不能再指向原来的内存，所以将 other 数据置为nullptr
        other.m_data = new char[1];
        other.m_data[0] = '\0';
        other.m_size = 0;
    }

    // 5. 析构函数
    ~myString()
    {
        delete[] m_data;
    }

    // 6. 拷贝赋值
    myString& operator=(const myString &other)
    {
        if (this != &other) // 避免自赋值
        {
            if (m_data != nullptr)
            {
                delete[] m_data;
            }
            m_data = new char[other.m_size + 1];
            strcpy(m_data, other.m_data);
            m_size = other.m_size;
        }
        return *this;
    }

    // 7. 移动赋值
    myString &operator=(myString &&other) noexcept
    {
        if (this != &other)
        {
            delete[] m_data; // 释放原有内存
            m_data = other.m_data;
            m_size = other.m_size;
            // 清空 other
            other.m_data = new char[1];
            other.m_data[0] = '\0';
            other.m_size = 0;
        }
        return *this;
    }

    // 8. 拼接（+=）
    myString& operator+=(const myString &other)
    {
        char *new_data = new char[m_size + other.m_size + 1];
        strcpy(new_data, m_data);
        strcat(new_data, other.m_data);
        delete[] m_data;
        m_data = new_data;
        m_size += other.m_size;
        return *this;
    }

    // 10. 获取C字符串
    const char *c_str() const {
        /*
        第一个 const：修饰函数的返回值类型，表示返回的指针指向的内容是不可修改的（即常量数据）
        第二个 const：表示该方法不会修改对象成员的值
        */
        return m_data; 
    }

    // 11. 获取长度
    size_t size() const { return m_size; }

private:
    char *m_data;  // 动态分配的字符数组（保存字符串）
    size_t m_size; // 字符串长度（不包含 '\0' 结尾符）
};

