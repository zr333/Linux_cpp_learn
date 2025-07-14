#include <iostream>
#include <atomic>
#include <pthread.h>
#include <chrono>
#include <thread>
#include "threadpool.h" 

// 全局计数器
std::atomic<int> task_count{0};
std::atomic<int> completed{0};

pthread_mutex_t output_mutex = PTHREAD_MUTEX_INITIALIZER;

// 测试任务类
class TestTask
{
public:
    void process()
    {
        // 模拟任务处理(约1ms工作量)
        volatile int dummy = 0;
        for (int i = 0; i < 100000; ++i)
        {
            dummy += i;
        }

        // 更新完成计数
        completed++;

        // 输出进度
        pthread_mutex_lock(&output_mutex);
        std::cout << "\rCompleted: " << completed << "/" << task_count << std::flush;
        pthread_mutex_unlock(&output_mutex);
    }
};

int main()
{
    // 默认参数
    int thread_num = 16;
    int max_tasks = 10000;
    
    // 创建线程池
    threadpool<TestTask> pool(thread_num, max_tasks);

    // 记录开始时间
    auto start = std::chrono::steady_clock::now();

    // 添加任务
    for (int i = 0; i < max_tasks; ++i)
    {
        TestTask *task = new TestTask();
        if (!pool.appendTask(task))
        {
            delete task;
            std::cerr << "Task queue full at " << i << " tasks" << std::endl;
            break;
        }
        task_count++;
    }

    // 等待所有任务完成
    while (completed < task_count)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // 计算耗时
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // 输出结果
    std::cout << "\n\nResults:"
              << "\nThreads: " << thread_num
              << "\nTasks: " << task_count
              << "\nTime: " << duration.count() << "ms"
              << "\nThroughput: " << (task_count * 1000.0 / duration.count()) << " tasks/sec\n";

    return 0;
}