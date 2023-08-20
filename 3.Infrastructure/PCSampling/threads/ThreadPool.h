//
// Created by Seagosoft on 8/19/2023.
//

#ifndef PC_SAMPLING_THREADPOOL_H
#define PC_SAMPLING_THREADPOOL_H

#include <pthread.h>
#include <unistd.h>

#define THREAD_POOL_SIZE 5
#define MAX_TASKS 20

typedef struct Task {
    void (*function)(void* arg);
    void* arg;
} Task;


class ThreadPool {

private:
    Task taskQueue[MAX_TASKS]{};
    int taskCount = 0;
    int taskHead = 0;
    int taskTail = 0;

    bool isThreadPoolEnd;

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;
    pthread_t threads[THREAD_POOL_SIZE]{};

public:
    ThreadPool();

    void cleanup_thread_pool();

    void init_thread_pool();

    void enqueue_task(void (*function)(void* arg), void* arg);

    void *thread_func(void *arg);
};

#endif //PC_SAMPLING_THREADPOOL_H
