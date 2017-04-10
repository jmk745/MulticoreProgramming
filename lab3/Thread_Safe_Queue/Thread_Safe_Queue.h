//
// Created by James on 3/25/17.
//

#ifndef LAB2_THREAD_SAFE_QUEUE_H
#define LAB2_THREAD_SAFE_QUEUE_H


#include <pthread.h>
#include <queue>
#include <climits>

template <class T>
class Thread_Safe_Queue{

public:
    Thread_Safe_Queue();
    ~Thread_Safe_Queue();
    int enqueue(T item);
    T dequeue();
    int isEmpty();
    int listen(T& element);

    //responsible for the statistics required in lab3
    int calculate_statistics();
    long double mean();
    long double median();
    long minimum();
    long maximum();

private:
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    std::queue<T> items;
    long _minimum;
    long _maximum;
    long _sum;
    long _count;
    long double _median;
};


#endif //LAB2_THREAD_SAFE_QUEUE_H
