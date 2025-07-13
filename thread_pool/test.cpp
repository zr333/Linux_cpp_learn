#include <iostream>
#include <unistd.h>
#include "threadpool.h"

// 定义一个简单的任务类
class Task
{
public:
    Task(int id) : m_id(id) {}

    void process()
    {
        std::cout << "Task " << m_id << " is being processed by thread " << pthread_self() << std::endl;

        delete this;
    }

private:
    int m_id;
};

int main()
{
    try
    {
        // 创建线程池，包含 4 个线程，最大请求数为 20
        threadpool<Task> pool(8, 10000);

        // 创建并添加任务到线程池
        for (int i = 0; i < 1000; ++i)
        {
            Task *task = new Task(i);
            if (pool.appendTask(task))
            {
                std::cout << "Task " << i << " added to the thread pool." << std::endl;
            }
            else
            {
                std::cout << "Failed to add Task " << i << " to the thread pool." << std::endl;
                delete task;
            }
        }

        // 主线程休眠一段时间，让线程池有时间处理任务
        sleep(3);


    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}