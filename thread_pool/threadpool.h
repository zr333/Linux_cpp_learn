#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <deque>
#include <vector>
#include <exception>
#include <cstdio>
#include <atomic>
#include "./locker.h"

// 定义线程池类，模板类用于代码复用，T为任务类
template <typename T>
class threadpool
{
public:
    threadpool(int thread_number = 8, int max_requests = 10000);
    ~threadpool();

    // 向请求队列中添加任务
    // 外部（比如主线程）通过调用 append() 方法，将新的任务提交到线程池处理。
    bool appendTask(T *request);

private:
    // 工作线程运行的函数，它不断地从任务队列中取出任务并执行
    static void *worker(void *arg);
    void run();

private:
    // 线程池数组，存放线程的容器，大小为m_thread_number
    std::vector<pthread_t> m_threads;

    // 线程池中的线程数
    int m_thread_number;

    // 请求队列,用于存放任务
    std::deque<T *> m_workqueue;

    // 请求队列中最多允许的、等待处理的请求的数量
    int m_max_requests;

    // 互斥锁
    locker m_queuelocker;

    // 信号量，用来判断是否有任务需要处理
    cond m_queuecond;

    // 是否结束线程,使用原子操作保证线程安全
    std::atomic<bool> m_stop;
};

template <typename T>
threadpool<T>::threadpool(int thread_number, int max_requests) : m_thread_number(thread_number), m_max_requests(max_requests), m_stop(false)
{
    if ((thread_number <= 0) || (max_requests <= 0))
    {
        throw std::exception();
    }
    // 创建线程池数组
    m_threads.resize(m_thread_number); // 自动分配内存,也不用手动释放内存

    // 创建线程池中的线程
    for (int i = 0; i < thread_number; i++)
    {
        printf("create the %dth thread\n", i);
        if (pthread_create(&m_threads[i], NULL, worker, this))
        {
            throw std::exception();
        }
    }
}

template <typename T>
threadpool<T>::~threadpool()
{
    m_stop = true;

    m_queuecond.broadcast();

    for (int i = 0; i < m_thread_number; ++i)
    {
        pthread_join(m_threads[i], nullptr);
    }
}

template <typename T>
bool threadpool<T>::appendTask(T *request)
{
    m_queuelocker.lock();

    // 如果请求队列已满，则返回false
    if (m_stop || m_workqueue.size() >= m_max_requests)
    {
        m_queuelocker.unlock();
        return false;
    }
    // 否则将任务添加到请求队列中
    m_workqueue.push_back(request);
    m_queuelocker.unlock();

    m_queuecond.signal();

    return true;
}

template <typename T>
void *threadpool<T>::worker(void *arg)
{
    threadpool *pool = static_cast<threadpool *>(arg);
    pool->run();
    return pool;
}

template <typename T>
void threadpool<T>::run()
{
    while (true)
    {

        m_queuelocker.lock();

        // 使用条件变量等待
        while (m_workqueue.empty() && !m_stop)
        {
            m_queuecond.wait(m_queuelocker.get());
        }

        if (m_stop && m_workqueue.empty())
        {
            m_queuelocker.unlock();
            break;
        }

        if (m_workqueue.empty())
        {
            m_queuelocker.unlock();
            continue;
        }

        T *request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();

        if (request)
            request->process();
    }
}
#endif