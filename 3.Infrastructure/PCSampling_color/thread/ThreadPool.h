//
// Created by Orlando Chen on 2023/07/07.
//

#ifndef PC_SAMPLING_THREADPOOL_H
#define PC_SAMPLING_THREADPOOL_H

#include "Worker.h"

class ThreadPool {
public:
    explicit ThreadPool(int numThreads) : stopFlag(false) {
        for (int i = 0; i < numThreads; ++i) {
            workers.emplace_back(Worker(
                    taskQueue, queueMutex, cv, stopFlag));
        }
    }

    ~ThreadPool() {
        // 设置停止标志并通知所有线程
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            stopFlag = true;
        }
        cv.notify_all();

        // 等待所有线程退出
        for (auto& worker : workers) {
            worker.join();
        }
    }

    template<typename F, typename... Args>
    void submit(F&& f, Args&&... args) {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            taskQueue.emplace(std::bind(std::forward<F>(f),
                    std::forward<Args>(args)...));
        }
        cv.notify_one();
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> taskQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    bool stopFlag;
};

//// 示例任务函数
//void printNumber(int number) {
//    std::cout << "Number: " << number << std::endl;
//}
//
//int main() {
//    ThreadPool threadPool(4); // 创建线程池，包含4个工作线程
//
//    // 提交任务到线程池
//    for (int i = 0; i < 10; ++i) {
//        threadPool.submit(printNumber, i);
//    }
//
//    // 等待所有任务完成并关闭线程池
//    // 线程池会自动释放资源，等待所有线程退出
//    return 0;
//}

#endif //PC_SAMPLING_THREADPOOL_H
