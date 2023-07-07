//
// Created by Orlando Chen on 2023/07/07.
//

#ifndef PC_SAMPLING_WORKER_H
#define PC_SAMPLING_WORKER_H

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class Worker {
public:
    Worker(std::queue<std::function<void()>>& taskQueue, std::mutex& queueMutex, std::condition_variable& cv, bool& stopFlag)
            : taskQueue(taskQueue), queueMutex(queueMutex), cv(cv), stopFlag(stopFlag) {}

    void operator() {
        while (true) {
            std::function<void()> task;

            // 获取任务
            {
                std::unique_lock<std::mutex> lock(queueMutex);

                // 等待任务队列非空或停止标志
                cv.wait(lock, [this] { return !taskQueue.empty() || stopFlag; });

                // 检查停止标志
                if (stopFlag && taskQueue.empty())
                    return;

                // 取出任务
                task = std::move(taskQueue.front());
                taskQueue.pop();
            }

            // 执行任务
            task();
        }
    }

private:
    std::queue<std::function<void()>>& taskQueue;
    std::mutex& queueMutex;
    std::condition_variable& cv;
    bool& stopFlag;
};

#endif //PC_SAMPLING_WORKER_H
