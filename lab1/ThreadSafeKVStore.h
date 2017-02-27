//
// Created by James on 2/25/17.
//


#ifndef THREADSAFEKVSTORE_H_
#define THREADSAFEKVSTORE_H_

#include <unordered_map>
#include <pthread.h>
#include <string>

template <typename K, typename V> class ThreadSafeKVStore{

    public:
        ThreadSafeKVStore< K, V>();
        ~ThreadSafeKVStore<K, V>();
        int insert(const K key, const V value);
        int accumulate(const K key, const V value);
        int lookup(const K key, V& value) const;
        int remove(const K key);
        typename std::unordered_map<K, V>::iterator begin();
        typename std::unordered_map<K, V>::iterator end();
        int size();

    private:
        std::unordered_map<K, V>* map;
        bool contains(const K key);
        pthread_mutex_t mutexLock;
};

//#include "ThreadSafeKVStore.cpp"

#endif //THREADSAFEKVSTORE_H_
