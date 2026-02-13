#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

std::queue<int> data_queue;  // 共享的数据队列
std::mutex queue_mutex;
std::condition_variable data_cond;

// 生产者线程
void producer() {
    for (int i = 0; i < 5; i++) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        data_queue.push(i);
        std::cout << "生产了数据: " << i << std::endl;
        data_cond.notify_one();
        std::this_thread::sleep_for(
            std::chrono::milliseconds(100)); // 稍微等一下
    }
}

// 消费者线程
void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        data_cond.wait(lock, []() { return !data_queue.empty(); });  // 等待队列有数据（避免虚假唤醒）
        int value = data_queue.front();
        data_queue.pop();
        std::cout << "消费了数据: " << value << std::endl;

        if (value == 4) break;  // 收到最后一个数据后退出
    }
}

int main() {
    std::thread prod(producer);
    std::thread cons(consumer);

    prod.join();
    cons.join();

    std::cout << "所有数据都生产和消费完毕！" << std::endl;
    return 0;
}
