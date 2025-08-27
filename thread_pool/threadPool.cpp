#include "threadPool.h"
#include <stdio.h>

ThreadPool::ThreadPool(int numThreads) : m_num_threads(numThreads), m_max_tasks(65535), m_stop(false) {
    
    //初始化
    pthread_mutex_init(&m_tasks_mutex, NULL);
    pthread_cond_init(&m_tasks_cond, NULL);

    m_threads.resize(m_num_threads); 

    for (int i = 0; i < m_num_threads; ++i) {

        printf("create the %dth thread\n", i);
        pthread_create(&m_threads[i], NULL, worker, this);
    }
}

ThreadPool::~ThreadPool() {
    m_stop = true; 

    pthread_cond_broadcast(&m_tasks_cond);
    for (int i = 0; i < m_num_threads; ++i) {
        pthread_join(m_threads[i], NULL);
    }
    pthread_mutex_destroy(&m_tasks_mutex);
    pthread_cond_destroy(&m_tasks_cond);
    
}

bool ThreadPool::addTask(task f){
    pthread_mutex_lock(&m_tasks_mutex);
    if(m_tasks.size() >= m_max_tasks) {
        pthread_mutex_unlock(&m_tasks_mutex);
        printf("task queue is full\n");
        return false;
    }
    m_tasks.emplace(f);
    pthread_mutex_unlock(&m_tasks_mutex);

    pthread_cond_signal(&m_tasks_cond);
    return true;
}

void* ThreadPool::worker(void* arg) {
    ThreadPool* pool = (ThreadPool*)arg;
    pool->run();
    return pool;
}

// 假设：task = std::function<void()> 或可移动可调用对象
void ThreadPool::run() {
    while(1) {
        task job; 

        pthread_mutex_lock(&m_tasks_mutex);

        // 等到：有任务可取 或 收到停止信号
        while (!m_stop && m_tasks.empty()) {
            pthread_cond_wait(&m_tasks_cond, &m_tasks_mutex); // 处理虚假唤醒
        }

        // 若要求停止，且已无任务，安全退出
        if (m_stop && m_tasks.empty()) {
            pthread_mutex_unlock(&m_tasks_mutex);
            break;
        }

        // 此时：要么有任务，要么 stop=true 但队列非空（继续把任务做完再退）
        job = std::move(m_tasks.front()); // 减少一次拷贝
        m_tasks.pop();

        pthread_mutex_unlock(&m_tasks_mutex);

        job();
        
        
    }
}
