//
// Created by James on 2/25/17.
//

#include "ThreadSafeKVStore.h"


template<typename K, typename V> ThreadSafeKVStore<K, V>::ThreadSafeKVStore(){
    map = new std::unordered_map<K, V>;
    mutexLock = PTHREAD_MUTEX_INITIALIZER;
}
template<typename K, typename V> ThreadSafeKVStore<K, V>::~ThreadSafeKVStore(){
    delete map;
    pthread_mutex_destroy(&mutexLock);
}


/*
 * Should insert the key-value pair if the key doesn’t exist in the hashmap, or update the value if it
 * does. Only return -1 if some fatal error occurs (which may not be possible at this point). Return 0
 * on success.
 */
template <typename K, typename V> int ThreadSafeKVStore<K, V>::insert(const K key, const V value){

    //
    // ---- Critical Zone: START ---
    //
    pthread_mutex_lock(&mutexLock);
    try {
        auto current = map->find(key);

        if (current != map->end())
            current->second = value;

        else
            map->insert(std::make_pair(key, value));

        pthread_mutex_unlock(&mutexLock);
        return 0;
    }
    catch(std::exception e){
        pthread_mutex_unlock(&mutexLock);
        return -1;
    }
    //
    // ---- Critical Zone: END ---
    //
}

/*
 * Like insert(), but if the key-value pair already exists, accumulate (i.e., add) the new value to the
 * existing value. This of course means that the templated V type must support the + operator.
 * Because addition on some data types (like strings) is not commutative, make sure that your
 * accumulation is in the form
 * stored_value = existing_stored_value + new_value;
 * The return values should mirror those for insert().
 */

template <typename K, typename V> int ThreadSafeKVStore<K, V>::accumulate(const K key, const V value){

    //
    // ---- Critical Zone: START ---
    //
    pthread_mutex_lock(&mutexLock);
    try {
        auto it = map->find(key);

        if (it != map->end())
            it->second = it->second + value;

        else
            map->insert(std::make_pair(key, value));

        pthread_mutex_unlock(&mutexLock);
        return 0;
    }
    catch(std::exception e){
        pthread_mutex_unlock(&mutexLock);
        return -1;
    }
    //
    // ---- Critical Zone: END ---
    //
}


/*
 * Return 0 if the key is present, -1 otherwise. If the key is present, fill the value variable (passed by
 * reference) with the associated value.
 */
template <typename K, typename V> int ThreadSafeKVStore<K, V>::lookup(const K key, V& value){

    //
    // ---- Critical Zone: START ---
    //
    pthread_mutex_lock(&mutexLock);

    bool found = false;
    for(auto it : *map){
        if(it.first == key){
            it.second = value;
            found = true;
            break;
        }
    }
    pthread_mutex_unlock(&mutexLock);
    return found ? 0:-1;

    //
    // ---- Critical Zone: END ---
    //

}

/*
 * Delete the key-value pair with key key from the hashmap, if it exists. Do nothing if it does not
 * exist. Return 0 on success; return -1 if there is some fatal error. The key not existing is a
 * “success” condition, not an error (why? Think about the invariants that the remove() operation
 * implies).
 */
template <typename K, typename V> int ThreadSafeKVStore<K, V>::remove(const K key){

    pthread_mutex_lock(&mutexLock);
    try{
        map->erase(key);
        pthread_mutex_unlock(&mutexLock);
        return 0;
    }
    catch(std::exception e){
        pthread_mutex_unlock(&mutexLock);
        return -1;
    }

}

/*
 * This method checks whether the unordered map already contains the value and returns value of type boolean.
 */
template<typename K, typename V> bool ThreadSafeKVStore<K, V>::contains(const K key) {

    pthread_mutex_lock(&mutexLock);
    for( auto it : *map){
        if(it.first == key){
            pthread_mutex_unlock(&mutexLock);
            return true;
        }
    }
    pthread_mutex_unlock(&mutexLock);
    return false;
}

/*
 * Returns the current size of the KV Store
 */
template  <typename K, typename V> int ThreadSafeKVStore<K, V>::size(){
    pthread_mutex_lock(&mutexLock);
    long temp = map->size();
    pthread_mutex_unlock(&mutexLock);
    return temp;

};

//The following methods are Not thread safe. Beware

template <typename K, typename V> typename std::unordered_map<K, V>::iterator ThreadSafeKVStore<K, V>::begin(){
    return map->begin();
};

template <typename K, typename V> typename std::unordered_map<K, V>::iterator ThreadSafeKVStore<K, V>::end(){
    return map->end();
};

