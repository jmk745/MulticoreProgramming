//
// Created by James on 3/25/17.
//

#include "Thread_Safe_Queue.h"


template <class T>
Thread_Safe_Queue<T>::Thread_Safe_Queue() {
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&condition, NULL);
}


template <class T>
Thread_Safe_Queue<T>::~Thread_Safe_Queue() {
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condition);
}

/*
 * Enqueue an item to the list safely.
 * method will return 1 if susccesfful and a 0 on a fault
 */
template <class T>
int Thread_Safe_Queue<T>::enqueue(T item) {
    try{
        pthread_mutex_lock(&mutex);
        items.push(item);
        pthread_cond_signal(&condition);
        pthread_mutex_unlock(&mutex);
        return 1;
    }
    catch(std::exception e){
        pthread_mutex_unlock(&mutex);
        return 0;
    }
}

/*
 * return a NULL object if empty, else return the next queued item
 */
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

/*
 * return 1/0 , true/false depending on whether the queue is empty
 */
template <class T>
int Thread_Safe_Queue<T>::isEmpty() {
    return items.empty();
}

/*
 * return 0/-1 depending if succesfully completed
 * Updates the variable passed in.
 */

template <class T>
int Thread_Safe_Queue<T>::listen(T& element) {
    try {
        pthread_mutex_lock(&mutex);
        while (items.empty()) {
            pthread_cond_wait(&condition, &mutex);
        }
        element = (T) items.front();
        items.pop();
        pthread_mutex_unlock(&mutex);
        return 0;
    }
    catch (std::exception e) {
        pthread_mutex_unlock(&mutex);
        return -1;
    }
}


/*
 * Return -1 on error and 0 on success
 */
template <class T>
int Thread_Safe_Queue<T>::calculate_statistics() {

    try {
        _minimum = INT_MAX;
        _maximum = 0;
        _median = 0;
        _sum = 0;
        _count = 0;

        pthread_mutex_lock(&mutex);

        std::vector <int> vect;
        _count = items.size();
        for (int i = 0; i < _count; i++) {

            T temp = items.front();
            vect.push_back(temp); //add to the vector

            //keep track of stats
            //GET MIN
            if (temp < _minimum) { _minimum = temp; }

            //GET MAX
            if (temp > _maximum) { _maximum = temp; }

            //GET SUM
            _sum += temp;

            //RESTORE QUEUE
            items.pop(); //remove item to access next in Queue
            items.push(temp); //insert element to keep original integrity and order of the queue
        }

        //GET MEDIAN
        if (_count > 0) {
            std::sort (vect.begin(), vect.end());
            int med_index = (_count / 2);
            if (_count % 2 == 0) { _median = vect[med_index]; }
            else { _median = (vect[med_index] + vect[med_index - 1]) / 2.0; }
        }

        pthread_mutex_unlock(&mutex);
        return 0;
    }
    catch (std::exception e) {
        pthread_mutex_unlock(&mutex);
        return -1;
    }
}


template <class T>
long Thread_Safe_Queue<T>::minimum() {
    return _minimum;
}


template <class T>
long Thread_Safe_Queue<T>::maximum() {
    return _maximum;
}

template <class T>
long double Thread_Safe_Queue<T>::mean() {
    long double num = (double) _sum;
    long double den = (double) _count;
    return (num/den);
}

template <class T>
long double Thread_Safe_Queue<T>::median() {
    return _median;
}





