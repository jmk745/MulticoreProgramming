//
// Created by James on 2/27/17.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string>
#include <vector>

#include "ThreadSafeKVStore.h"
#include "ThreadSafeListenerQueue.h"


struct wrapper{
    ThreadSafeKVStore<std::string, int>* store;
    ThreadSafeListenerQueue<int>* queue;
    int* thread_counter;
    int* missing_count;
    double* thread_array;
    pthread_mutex_t* mutex_lock;
};


void* testThread(void* obj){

    clock_t thread_start_time = clock();

    //Unpack the passed in object wrapper into the needed KV_store and queue
    struct wrapper* wrappedObject = (struct wrapper*) obj;
    ThreadSafeKVStore<std::string, int>* KV_store = (ThreadSafeKVStore<std::string, int>*)  wrappedObject->store;
    ThreadSafeListenerQueue<int>* listener_queue = (ThreadSafeListenerQueue<int>*) wrappedObject->queue;
    int* thread_count = wrappedObject->thread_counter;
    int* missing_count = wrappedObject->missing_count;
    double* thread_run_time = wrappedObject->thread_array;
    pthread_mutex_t* count_mutex = wrappedObject->mutex_lock;


    //Get a random seed and initialize a control vector in order to check that all threads are synchronized;
    srand(time(NULL));
    std::vector<std::pair<std::string, int>> control_vector;

    //Step 1 and Step 2
    //
    //
    long run_sum=0;
    std::string user_key;
    int random_key, random_value;
    int accumulate_or_lookup=90;
    int missing_lookups=0;
    for(int i=0; i<10000; i++){

//      accumulate_or_lookup = (rand() % 100);
        random_key = (rand() % 500);
        user_key = "user" + std::to_string(random_key);
        //Generate a number from 0 to 100 to create a 20% and 80% action ratio
        if(accumulate_or_lookup <= 20 && control_vector.size() != 0){

            long random_index = (rand() % control_vector.size());
            if(KV_store->lookup(control_vector[random_index].first, control_vector[random_index].second) == -1)
                missing_lookups++;
        }
        else{
            random_value = (rand() % 513) - 256;
            run_sum = run_sum + random_value;

            //control_vector.push_back(make_pair(user_key, random_key));

            if(KV_store->accumulate(user_key, random_value)==-1)
                printf("ERROR!!!!!!!!\n");
        }
    }
    time_t thread_end_time = clock();

    pthread_mutex_lock(count_mutex);
    thread_run_time[*thread_count] = difftime(thread_end_time, thread_start_time) / CLOCKS_PER_SEC;
    *missing_count += missing_lookups;

    printf("Thread %i\nCompleted at:       \t%sRun time:     \t\t%f seconds\nPushed run_sum:     \t%li\nNumber of Missing Keys: %i\n\n\n",
           *thread_count, asctime(localtime(&thread_end_time)), thread_run_time[*thread_count], run_sum, missing_lookups);

    (*thread_count)++;
    pthread_mutex_unlock(count_mutex);

    listener_queue->push(run_sum);

    pthread_exit(NULL);
}

