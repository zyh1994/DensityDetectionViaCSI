//
// Created by Seagosoft on 8/19/2023.
//

#include "ThreadPool.h"


#include <pthread.h>
#include <unistd.h>

#define THREAD_POOL_SIZE 5
#define MAX_TASKS 20






void* thread_func(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        while (taskCount == 0) {
            pthread_cond_wait(&cond_var, &mutex);
        }

        Task task = taskQueue[taskHead];
        taskHead = (taskHead + 1) % MAX_TASKS;
        taskCount--;

        pthread_mutex_unlock(&mutex);

        (*(task.function))(task.arg);
    }
}

void enqueue_task(void (*function)(void* arg), void* arg) {
    pthread_mutex_lock(&mutex);

    taskQueue[taskTail].function = function;
    taskQueue[taskTail].arg = arg;
    taskTail = (taskTail + 1) % MAX_TASKS;
    taskCount++;

    pthread_cond_signal(&cond_var);
    pthread_mutex_unlock(&mutex);
}

void init_thread_pool() {
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_create(&threads[i], NULL, thread_func, NULL);
    }
}

void cleanup_thread_pool() {
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_cancel(threads[i]);
        pthread_join(threads[i], NULL);
    }
}

void sample_task(void* arg) {
    printf("Executing Task %d\n", *(int*)arg);
    sleep(1);
}

int main() {
    init_thread_pool();

    for (int i = 0; i < 10; i++) {
        int* arg = malloc(sizeof(int));
        *arg = i;
        enqueue_task(sample_task, arg);
    }

    sleep(12); // Give it some time to execute the tasks

    cleanup_thread_pool();
    return 0;
}
