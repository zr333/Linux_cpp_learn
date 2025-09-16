#include <cstring>
#include <iostream>

using namespace std;

class myString {
  private:
    char *data;
    unsigned int length;

  public:
    // 默认构造
    myString() : data(new char[1]), length(0) {
        data[0] = '\0'; // 初始化为空字符串
    }

    // 从c字符串构造
    myString(const char *str) {
        if (str) {
            length = strlen(str);
            this->data = new char[length + 1];
            strcpy(this->data, str);
        } else { // 处理传入 nullptr 的情况
            length = 0;
            this->data = new char[1];
            this->data[0] = '\0';
        }
    }
    // 拷贝构造(深拷贝)
    myString(const myString &str) {
        length = str.length;
        data = new char[length + 1];
        strcpy(data, str.data);
    }
    // 析构
    ~myString() {
        if (data) {
            delete[] data;
            data = nullptr;
        }
    }
    // 拷贝赋值(深拷贝)-类对象版
    myString &operator=(const myString &str) {
        if (this != &str) { // 避免自赋值
            delete[] data;
            length = str.length;
            data = new char[length + 1];
            strcpy(data, str.data);
        }
        return *this;
    }
    // 拷贝赋值(深拷贝)-c风格字符串版
    myString &operator=(const char *str) {
        delete[] data;

        length = strlen(str);
        data = new char[length + 1];
        strcpy(data, str);
        return *this;
    }
    // 拼接
    myString& operator+=(const myString &str) { 
        length += str.length;
        char *tmp = new char[length + str.length + 1];
        strcpy(tmp, data);
        strcat(tmp, str.data);
        delete[] data;
        data = tmp;
        return *this;

    }
    // 重载[]
    char& operator[](int index) {
        return data[index];
    }

    const char *c_str() const {
        /*
        第一个
        const：修饰函数的返回值类型，表示返回的指针指向的内容是不可修改的（即常量数据）
        第二个 const：表示该方法不会修改对象成员的值
        */
        return data;
    }
};

int main() {
    myString str1("hello world");
    myString str2(str1);
    myString str3 = str2;
    myString str4;
    str4 += str3;
    
    cout << str2.c_str() << endl;
    cout << str3.c_str() << endl;
    cout << str4.c_str() << endl;
    cout << str4[3] << endl;

    return 0;
}