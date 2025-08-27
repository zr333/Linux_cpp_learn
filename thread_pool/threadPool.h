#pragma once
#include <functional>
#include <pthread.h>
#include <queue>
#include <vector>


using task = std::function<void()>; // 定义 Task 类型

class ThreadPool {
  public:
    ThreadPool(int num_threads);

    ~ThreadPool();

    // 面向用户的添加任务
    bool addTask(task);

  private:
    static void* worker(void* arg);
    void run();

  private:
    std::vector<pthread_t> m_threads; // 线程池，用数组存储
    std::queue<task> m_tasks; // 任务队列

    int m_max_tasks;   // 任务队列中的最大任务数
    int m_num_threads;  // worker总数
    bool m_stop; // 是否终止线程池

    pthread_cond_t m_tasks_cond; // 线程条件等待
    pthread_mutex_t
        m_tasks_mutex; // 为任务加锁防止一个任务被两个线程执行等其他情况
};