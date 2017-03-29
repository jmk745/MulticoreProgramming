//
// Created by James on 3/25/17.
//

#include "Thread_Pool.h"



Thread_Pool::Thread_Pool() {
    tasks = new Thread_Safe_Queue<queue_container*>();
    pthread_cond_init(&thread_condition_variable, NULL);
    pthread_cond_init(&destroy_condition_variable, NULL);
    pthread_mutex_init(&mutex, NULL);
}


void Thread_Pool::init(int size){
    thread_pool_size = size;
    thread_pool_status = 1; // 1 represents True or Running ... 0 is shut down
    pool = new pthread_t[size];
    //create the threads for this thread pool
    for(int i=0; i<thread_pool_size; i++) {
        pthread_create(&pool[i], NULL, &wrapper_function, this);
    }
}


Thread_Pool::~Thread_Pool() {

//    if( !tasks->isEmpty() ){
//        pthread_cond_wait(&destroy_condition_variable, &mutex);
//    }
    pthread_mutex_lock(&mutex);
    thread_pool_status = 0; //shut down status
    pthread_mutex_unlock(&mutex);

    pthread_cond_broadcast(&thread_condition_variable);  //wake up any sleeping threads
    for (int i=0; i<thread_pool_size; i++) {
        pthread_join(pool[i], NULL);
//        printf("TID: %d\n", pool[i]);
        pthread_cond_broadcast(&thread_condition_variable);  //wake up any sleeping threads
    }

    delete pool;
    pthread_cond_destroy(&thread_condition_variable);
    pthread_cond_destroy(&destroy_condition_variable);
    pthread_mutex_destroy(&mutex);
}


void Thread_Pool::work_thread() {

    pthread_mutex_lock(&mutex);
    while(thread_pool_status != 0) {

        while(thread_pool_status==1 && tasks->isEmpty()) {
            printf("red for %ld\n", (long)pthread_self());
            pthread_cond_signal(&destroy_condition_variable);
            pthread_cond_wait(&thread_condition_variable, &mutex);
            printf("GREEN for %ld\n", (long)pthread_self());
        }

        pthread_mutex_unlock(&mutex);

        //perform task on passed in data set
        queue_container* next_task;
        if( (next_task = tasks->dequeue()) != NULL ){
            printf("Going to perform task\n");
            next_task->fn(next_task->data);
        }
        delete next_task;

        pthread_mutex_lock(&mutex);
    }
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}


void Thread_Pool::work_thread2() {

    while (thread_pool_status || !tasks->isEmpty()) {

        pthread_mutex_lock(&mutex);

        while ( tasks->isEmpty() && thread_pool_status ) {
            printf("%ld is Waiting...\n", (long)pthread_self());
            pthread_cond_wait(&thread_condition_variable, &mutex);
            printf("%ld is Running...\n", (long)pthread_self());
        }

        pthread_mutex_unlock(&mutex);

        queue_container* next_task = tasks->dequeue();

        //Do the work
        printf("%ld is Working...\n", (long)pthread_self());

        if(next_task != NULL)
            next_task->fn(next_task->data);

        delete next_task;
    }
            printf("%ld is Terminating...\n", (long)pthread_self());
            pthread_exit(0);
}


void test_fn(){
    printf("DOING WORK\n");
}


/*
 * Add the prewritten task/function and include that data that it will be operationg on.
 * This method packages those 2 items into one container and inserts it onto a queue.
 * Then the spwaned threads will unpackage the contianer nd perform the speciied actions
 */
int Thread_Pool::add_task( void* (*task)(void*), void* data ) {
    try{
        //Do not add any additonal tasks to the thread pool since it is shutting down.
        if(thread_pool_status==0)
            return -1;

        queue_container* container = new queue_container();
        container->fn = task;
        container->data = data;
        tasks->enqueue(container);
        pthread_cond_signal(&thread_condition_variable);
        return 1;
    }
    catch(std::exception e){
        printf("Error: in adding task\n");
        return 0;
    }
}

//wrapper function idea from https://github.com/bilash/threadpool/blob/master/ThreadPool.cpp
void* wrapper_function(void* argument){
    Thread_Pool* thread_pool = (Thread_Pool*) argument;
    thread_pool->work_thread2(); //execute the member function for the thread
    pthread_exit(0);
}
