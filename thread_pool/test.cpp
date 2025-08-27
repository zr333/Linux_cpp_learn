#include "threadPool.h"
#include <iostream>
#include <unistd.h> 

void taskFunction(int id) {
    std::cout << "Task " << id << " is running on thread " << pthread_self()
              << std::endl;
    sleep(1); // 模拟耗时操作
    std::cout << "Task " << id << " completed on thread " << pthread_self() << std::endl;
}

int main() {
    // 创建一个包含4个线程的线程池
    ThreadPool pool(4);

    // 添加10个任务到线程池
    for (int i = 0; i < 10; ++i) {
        pool.addTask([i]() { taskFunction(i); });
        sleep(1); 
    }

    return 0;
}
