//
// Created by James on 2/26/17.
//


#ifndef THREADSAFELISTENERQUEUE_H_
#define THREADSAFELISTENERQUEUE_H_

#include <list>
#include <pthread.h>

template <typename T> class ThreadSafeListenerQueue {

    public:
        ThreadSafeListenerQueue();
        ~ThreadSafeListenerQueue();
        int push(const T element);
        int pop(T& element);
        int listen(T& element);

    private:
        std::list<T>* queue;
        pthread_mutex_t mutexLock;
};


#endif //THREADSAFELISTENERQUEUE_H
