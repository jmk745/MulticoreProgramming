//
// Created by James on 3/25/17.
//

#ifndef LAB2_THREADPOOL_H
#define LAB2_THREADPOOL_H


#include <pthread.h>
#include <stdlib.h>
#include <cstdio>
#include "../Thread_Safe_Queue/Thread_Safe_Queue.h"
#include "../Thread_Safe_Queue/Thread_Safe_Queue.cpp"


//container to be placed in the thread safe queue
typedef struct{
    void* (*fn)(void*); //holds function that will perform expected job request
    void* data;   // holds the data that the job request will use.
} queue_container;

class Thread_Pool {

public:
    Thread_Pool();
    ~Thread_Pool();
    void init(int);
    int add_task( void* (*task)(void*), void* data );
    void work_thread();
    void work_thread2();

private:
    int thread_pool_size;
    int thread_pool_status;
    pthread_t* pool;
    Thread_Safe_Queue <queue_container*> *tasks;
    pthread_mutex_t mutex;
    pthread_cond_t thread_condition_variable;
};

void* wrapper_function(void*);
void test_fn();


#endif //LAB2_THREADPOOL_H
