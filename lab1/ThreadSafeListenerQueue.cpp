//
// Created by James on 2/26/17.
//

#include "ThreadSafeListenerQueue.h"

template <typename T> ThreadSafeListenerQueue<T>::ThreadSafeListenerQueue() {
    queue = new std::list<T>;
    pthread_mutex_init(&mutexLock, NULL);
}

template <typename T> ThreadSafeListenerQueue<T>::~ThreadSafeListenerQueue() {
    delete queue;
}

/*
 * Should push the element onto the front of the list, so that it will be the last of the items currently
 * on the queue to be removed. Only return -1 if some fatal error occurs (which may not be possible
 * at this point). Return 0 on success
 */
template <typename T> int ThreadSafeListenerQueue<T>::push(const T element){
    pthread_mutex_lock(&mutexLock);
    try{
        queue->push_back(element);
        pthread_mutex_unlock(&mutexLock);
        return 0;
    }catch(std::exception e){
        pthread_mutex_unlock(&mutexLock);
        return -1;
    }
}

/*
 * Pop the least-recently inserted element from the queue and fill in the passed-by-reference
 * variable element with its contents, if the queue was not empty. Return 0 if this was successful;
 * return 1 if the queue was empty, or -1 if some fatal error occurred
 */
template <typename T> int ThreadSafeListenerQueue<T>::pop(T& element){
    pthread_mutex_lock(&mutexLock);
    try{
        element = queue->back();
        queue->pop_front();
        pthread_mutex_unlock(&mutexLock);
        return 0;
    }catch(std::exception e){
        pthread_mutex_unlock(&mutexLock);
        return -1;
    }
}

/*
 * Similar to pop(), but block until there is an element to be popped. Return 0 if an element was
 * returned, or -1 if some fatal error occurred.
 */
template <typename T> int ThreadSafeListenerQueue<T>::listen(T& element){
    try {
        while (queue->empty()){} //wait until list has an element;
        return ThreadSafeListenerQueue<T>::pop(element);
    }catch(std::exception e){
        return -1;
    }
}



