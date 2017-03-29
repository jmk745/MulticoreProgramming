//
// Created by James on 3/25/17.
//

#include "Thread_Safe_Queue.h"


template <class T>
Thread_Safe_Queue<T>::Thread_Safe_Queue() {
    pthread_mutex_init(&mutex, NULL);
}

template <class T>
Thread_Safe_Queue<T>::~Thread_Safe_Queue() {
    pthread_mutex_destroy(&mutex);
}

template <class T>
int Thread_Safe_Queue<T>::enqueue(T item) {
    try{
        pthread_mutex_lock(&mutex);
        items.push(item);
        pthread_mutex_unlock(&mutex);
        return 1;
    }
    catch(std::exception e){
        pthread_mutex_unlock(&mutex);
        return 0;
    }
}

template <class T>
T Thread_Safe_Queue<T>::dequeue() {
    try{
        T return_item = NULL;
        pthread_mutex_lock(&mutex);
        if(!items.empty()){
            return_item = (T) items.front();
            items.pop();
        }
        pthread_mutex_unlock(&mutex);
        return return_item;
    }
    catch(std::exception e){
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
}

template <class T>
int Thread_Safe_Queue<T>::isEmpty() {
    return items.empty();
}






