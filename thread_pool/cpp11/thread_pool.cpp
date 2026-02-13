#include "thread_pool.h"

#include <future>
#include <iostream>

threadPool::threadPool(int num) : threadNum(num), m_stop(false) {
    for (int i = 0; i < threadNum; i++) {
        m_threads.emplace_back(&threadPool::worker, this);
    }
}
threadPool::~threadPool() {
    m_stop = true;

    m_cond.notify_all();
    for (std::thread& th : m_threads) {
        if (th.joinable()) {
            std::cout << "******** 线程 " << th.get_id() << " 将要退出了..." << std::endl;

            th.join();
        }
    }
}

void threadPool::worker() {
    while (true) {  // 必须使用true，不然会丢任务
        std::function<void()> task = nullptr;

        {
            std::unique_lock<std::mutex> lock(m_mutex);

            // while(m_tasks.empty() && !m_stop){ // 如果任务队列为空且线程池没有停止，等待
            //     m_cond.wait(lock);
            // }

            /*
            谓词版（简洁版）：
            相当于做了这几步：
            检查谓词：
            如果 lambda表达式 返回 true（这里是 m_stop == true 或 m_tasks 非空），wait立即返回，不会睡眠。
            如果 lambda表达式 返回 false，则自动释放 lock，把线程挂起，直到被 notify_one() 或 notify_all()
            唤醒，再次尝试。 被唤醒后： 线程重新拿到 lock，再次调用 predicate()。 如果 predicate() 依旧为
            false，继续挂起（避免虚假唤醒）。 如果 predicate() 为 true，返回并继续执行下面的代码
            */
            m_cond.wait(lock, [this]() { return !m_tasks.empty() || m_stop; });

            // 若要求停止，且已无任务，安全退出
            if (m_stop && m_tasks.empty()) {
                // lock.unlock(); // 不用显示解锁
                return;
            }
            // 此时：要么有任务，要么 stop=true 但队列非空（继续把任务做完再退）
            task = std::move(m_tasks.front());
            m_tasks.pop();
        }
        // 最后再判断一下
        if (task) {
            task();
        }
    }
}

/*
简单版本： 只能接受一个 function 类型，返回值为 void，并且没有入参。那如果我们想提交一个返回值为 int，也有
int，作为入参的函数这样就行不通了。
*/
void threadPool::addTask(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_stop) {
            throw std::runtime_error("threadPool stopped");  // 线程池已停止,不能入队
        }
        m_tasks.emplace(task);
    }
    m_cond.notify_one();
}


