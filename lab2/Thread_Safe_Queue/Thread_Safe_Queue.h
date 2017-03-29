//
// Created by James on 3/25/17.
//

#ifndef LAB2_THREAD_SAFE_QUEUE_H
#define LAB2_THREAD_SAFE_QUEUE_H


#include <pthread.h>
#include <queue>

template <class T>
class Thread_Safe_Queue{

public:
    Thread_Safe_Queue();
    ~Thread_Safe_Queue();
    int enqueue(T item);
    T dequeue();
    int isEmpty();

private:
    pthread_mutex_t mutex;
    std::queue<T> items;
};


#endif //LAB2_THREAD_SAFE_QUEUE_H
