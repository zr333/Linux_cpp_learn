#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class threadPool {
public:
    threadPool(int numThreads);
    ~threadPool();

    void addTask(std::function<void()> task);

        // 升级版本： 允许提交任意可调用对象，返回值任意，入参任意(不是很清楚，之后再理解)
    //     template <typename F, typename... Args>
    //     auto addTaskResult(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
    //     using return_type = decltype(f(args...));
    //     auto task_ptr =
    //         std::make_shared<std::packaged_task<return_type()>>(  // 因为 packaged_task 无法拷贝构造，所以用 make_shared
    //             std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    //     std::future<return_type> res = task_ptr->get_future();
    //     {
    //         std::unique_lock<std::mutex> lock();
    //         tasks_.emplace([task_ptr]() { (*task_ptr)(); });
    //     }
    //     m_cond.notify_one();
    //     return res;
    // }

private:
    void worker();

    std::vector<std::thread> m_threads;         // 线程池，用数组存储
    std::queue<std::function<void()>> m_tasks;  // 任务队列
    int threadNum;                              // 线程数量
    std::mutex m_mutex;
    std::condition_variable m_cond;
    std::atomic<bool> m_stop;
};

#endif