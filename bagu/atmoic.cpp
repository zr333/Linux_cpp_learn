#include <atomic>
#include <iostream>
#include <thread>
using namespace std;

int normal_counter = 0;         // 普通 int
atomic<int> atomic_counter(0);  // 原子 int

void work_normal() {
    for (int i = 0; i < 1000000; i++) {
        normal_counter++;  // 非原子操作
    }
}

void work_atomic() {
    for (int i = 0; i < 1000000; i++) {
        atomic_counter++;  // 原子操作
    }
}

int main() {
    thread t1(work_normal);
    thread t2(work_normal);
    t1.join();
    t2.join();
    cout << "普通 int 结果: " << normal_counter << " (期望 2000000)" << endl;

    thread t3(work_atomic);
    thread t4(work_atomic);
    t3.join();
    t4.join();
    cout << "atomic<int> 结果: " << atomic_counter << " (期望 2000000)" << endl;

    return 0;
}
