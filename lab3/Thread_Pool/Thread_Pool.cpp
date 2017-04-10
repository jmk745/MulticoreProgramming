//
// Created by James on 3/25/17.
//

#include "Thread_Pool.h"



/*
 * Constructor: responsible for initiliazing the conditon variable, mutex, as well
 * as the tasks queue
 */
Thread_Pool::Thread_Pool() {
    tasks = new Thread_Safe_Queue<queue_container*>();
    pthread_cond_init(&thread_condition_variable, NULL);
    pthread_mutex_init(&mutex, NULL);
}

/*
 * Initialize the thread pool with the inputed number of threads
 * Ensure that the threads enter their working_thread routine
 */
void Thread_Pool::init(int size){
    thread_pool_size = size;
    thread_pool_status = 1; // 1 represents True or Running ... 0 is shut down
    pool = new pthread_t[size];
    //create the threads for this thread pool
    for(int i=0; i<thread_pool_size; i++) {
        pthread_create(&pool[i], NULL, &wrapper_function, this);
    }
}


/*
 * Chnage status of the thread pool to off ad wait for all threads to complete their tasks as well as any
 * remianing queued tasks. (Chnaging the thread_pool_status also prevents users from adding any additonal tasks to the
 * the task queue)
 */
Thread_Pool::~Thread_Pool() {

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
    pthread_mutex_destroy(&mutex);
}



//This thread_function keeps the thread running until the pool status is marked for deletion
// and there are tasks queued.
//Thus, the thread pool will never end prematurely unless an exception/error occurs
void Thread_Pool::work_thread() {


    while (thread_pool_status || !tasks->isEmpty()) {

        //
        // Critical Zone --- Start ---
        //
        pthread_mutex_lock(&mutex);
        while ( tasks->isEmpty() && thread_pool_status ) {
            pthread_cond_wait(&thread_condition_variable, &mutex); //sleep until next taks is assigned
        }
        pthread_mutex_unlock(&mutex);
        //
        // Critical Zone --- End ---
        //

        // Dequeue the pending job and exeute it on the attached data
        queue_container* next_task = tasks->dequeue();
        if(next_task != NULL) {
            printf("%ld is Working...\n", (long)pthread_self());
            next_task->fn(next_task->data);
        }

        delete next_task;
    }
//            printf("%ld is Terminating...\n", (long)pthread_self());
            pthread_exit(0);
}


/*
 * Add the pre-written task/function and include that data that it will be operating on.
 * This method packages those 2 items into one container and inserts it onto a queue.
 * Then the spawned threads will unpackage the container nd perform the specified actions
 * Method will stop adding tasks once the thread_pool is marked for destruction
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
//
void* wrapper_function(void* argument){
    Thread_Pool* thread_pool = (Thread_Pool*) argument;
    thread_pool->work_thread(); //execute the member function for the thread
    pthread_exit(0);
}
