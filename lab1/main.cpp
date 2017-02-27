#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string>
#include <ctime>
#include <iostream>
#include <vector>

#include "ThreadSafeKVStore.h"
#include "ThreadSafeKVStore.cpp"
#include "ThreadSafeListenerQueue.h"
#include "ThreadSafeListenerQueue.cpp"
#include "testThread.cpp"


using namespace std;

const int MAX_NUMBER_OF_THREADS = 20;


int main(int argc, char** argv){

    int num_of_threads;

    //Code responsible for getOpt and ensuring proper argument entry
    int option;
    int nFlag=0;
    while( (option = getopt(argc, argv, "n:")) != -1){

        switch(option){
            case 'n':
                num_of_threads = atoi(optarg);
                nFlag++;
                break;
            default:
                printf("Error: Missing -n <Number of threads>\n");
                return -1;
        }
    }

    if(argc != 3 || nFlag==0){
        printf("Improper number of arguments. Please run ./exec -n <N>\n");
        return -1;
    }

    if(num_of_threads < 1 || num_of_threads > MAX_NUMBER_OF_THREADS) {
        printf("Error: Invalid number of threads. Please enter a number between 1 and %i inclusively.\n", MAX_NUMBER_OF_THREADS);
        return -1;
    }


    //The official start of the program
    time_t program_start_time = clock();

    printf("Program has started running at: %s\n", ctime(&program_start_time));
    printf("----PROGRAM IS RUNNING SUCCESSFULLY-----\n\n\n");




    // --- Wrapper Object Creation ---
    //Wrapping up the DS into a wrapper object to be passed in to the thread function
    //Also adding the pointers to the thread count and thread runtime array. These 2 are to prevent from having them as global variables.

    //Initializations...
    ThreadSafeKVStore<string, int>* ts_kv_store = new ThreadSafeKVStore<string, int>::ThreadSafeKVStore<string, int>();
    ThreadSafeListenerQueue<int>* ts_listener_queue = new ThreadSafeListenerQueue<int>::ThreadSafeListenerQueue<int>();
    int thread_count=0; //counter used for counting the current thread during print out
    int missing_count=0; //num of keys that were missed during look up
    double  thread_run_time[MAX_NUMBER_OF_THREADS]; //array storing the run time for each thread
    pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER; // mutex for threads accessing the thread & missing counters

    //Declaring all elements of object
    struct wrapper objWrapper;
    objWrapper.store = ts_kv_store;
    objWrapper.queue = ts_listener_queue;
    objWrapper.thread_counter = &thread_count;
    objWrapper.thread_array = thread_run_time;
    objWrapper.missing_count = &missing_count;
    objWrapper.mutex_lock = &count_mutex;



    // --- Running Multi Threads ---
    //Creating threads and storing them in an array
    pthread_t threads[MAX_NUMBER_OF_THREADS];
    for(int i=0; i<num_of_threads; i++){
        pthread_create(&threads[i], NULL, testThread, &objWrapper);
    }
    printf("Started threads..\n\n");


    // --- Listen for all the treads to finish ---
    // Will stay in this loop until all threads push in their calculated sum into the queue.
    //Also sums up the totals for a overall total which will be verified with a checkSum
    int count=0, retrieved_value, total=0;
    while(count<num_of_threads){
        ts_listener_queue->listen(retrieved_value);
        total = total + retrieved_value;
        count++;
    }

    // --- Calculating the Check Sum ---
    // Iterate through the KV_store DS and used the recorded sum to compare against the sum achieved from
    // the ThreadSafe Queue.. Needed to check for thread synchronization
    long long check_sum=0;
    for(auto it = ts_kv_store->begin(); it != ts_kv_store->end(); it++){
        check_sum += it->second;
    }
    time_t program_end_time = clock();


    // --- Summation of Threa Run Times
    // Iterate through the array to get the sum of all the values. Used later for finding the average
    double total_thread_time;
    for(int i=0; i<num_of_threads; i++){
        total_thread_time+=thread_run_time[i];
    }

    //Print out Conclusion of the test threads
    printf("\nAnalysis\n");
    printf("---------------------------------------\n");
    printf("Sum from Queue:          %i\n", total);
    printf("Sum from KV Store:       %lli\n", check_sum);
    printf("Total Num of Missed Keys %i\n", missing_count);
    printf("Number of threads:       %i\n", num_of_threads);
    printf("Average thread run time: %f secs\n", total_thread_time/num_of_threads);
    printf("Whole test run time:     %f secs\n\n", difftime(program_end_time, program_start_time) / CLOCKS_PER_SEC);

    return 0;
}
