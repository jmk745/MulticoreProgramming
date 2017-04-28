//
// Created by James on 3/25/17.
//

#ifndef LAB2_THREAD_SAFE_KV_STORE_2_H
#define LAB2_THREAD_SAFE_KV_STORE_2_H



#include <unordered_map>
#include <pthread.h>
#include <string>
#include <random>

template <class K, class V> class Thread_Safe_KV_Store_2{

public:
    Thread_Safe_KV_Store_2< K, V>();
    ~Thread_Safe_KV_Store_2<K, V>();
    int insert(const K key, const V value);
    int accumulate(const K key, const V value);
    int lookup(const K key, V& value);
    int remove(const K key);
    int remove_random();


    typename std::unordered_map<K, V>::iterator begin();
    typename std::unordered_map<K, V>::iterator end();
    int size();

private:
    std::unordered_map<K, V>* map;
    bool contains(const K key);
    pthread_rwlock_t rw_lock;
};


#endif //LAB2_THREAD_SAFE_KV_STORE_2_H
