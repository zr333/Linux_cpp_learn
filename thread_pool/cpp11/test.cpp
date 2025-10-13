#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

#include "thread_pool.h"

using namespace std;

// 创建互斥锁用于同步输出
mutex output_mutex;

void calc(int x, int y, int taskId) {
    // 记录开始时间
    auto start = chrono::high_resolution_clock::now();

    // 执行计算
    int res = x + y;

    // 同步输出结果
    {
        lock_guard<mutex> lock(output_mutex);
        cout << "Task " << taskId << ": calc(" << x << ", " << y << ") = " << res << " [thread: " << pthread_self()
             << "]" << endl;
    }

    // 模拟耗时操作
    this_thread::sleep_for(chrono::seconds(2));

    // 记录结束时间并输出
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    {
        lock_guard<mutex> lock(output_mutex);
        cout << "Task " << taskId << " completed in " << duration << " ms" << endl;
    }
}

int main() {
    cout << "=== Thread Pool Test ===" << endl;
    cout << "Creating thread pool with 4 threads..." << endl;

    // 创建一个包含4个线程的线程池
    threadPool pool(4);

    cout << "Submitting 10 tasks to thread pool..." << endl;

    // 添加10个任务到线程池
    for (int i = 0; i < 10; ++i) {
        // 使用 bind 绑定参数并添加到线程池
        auto func = bind(calc, i, i * 3, i);
        pool.addTask(func);
    }

    cout << "All tasks submitted. Press Enter to exit..." << endl;

    // 等待用户输入再退出
    cin.get();

    cout << "Program terminated." << endl;

    return 0;
}