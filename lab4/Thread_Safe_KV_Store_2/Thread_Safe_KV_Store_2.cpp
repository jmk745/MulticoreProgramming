//
// Created by James on 3/25/17.
//

#include "Thread_Safe_KV_Store_2.h"


template<typename K, typename V> Thread_Safe_KV_Store_2<K, V>::Thread_Safe_KV_Store_2(){
    map = new std::unordered_map<K,V>();
    pthread_rwlock_init(&rw_lock, NULL);
}
template<typename K, typename V> Thread_Safe_KV_Store_2<K, V>::~Thread_Safe_KV_Store_2(){
    delete map;
    pthread_rwlock_init(&rw_lock, NULL);
}


/*
 * Should insert the key-value pair if the key doesn’t exist in the hashmap, or update the value if it
 * does. Only return -1 if some fatal error occurs (which may not be possible at this point). Return 0
 * on success.
 */
template <typename K, typename V> int Thread_Safe_KV_Store_2<K, V>::insert(const K key, const V value){

    //
    // ---- Critical Zone: START ---
    //
    pthread_rwlock_wrlock(&rw_lock);
    try {
        auto current = map->find(key);
        if (current != map->end())
            current->second = value;

        else
            map->insert(std::make_pair(key, value));

        pthread_rwlock_unlock(&rw_lock);
        return 0;
    }
    catch(std::exception e){
        pthread_rwlock_unlock(&rw_lock);
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

template <typename K, typename V> int Thread_Safe_KV_Store_2<K, V>::accumulate(const K key, const V value){

    //
    // ---- Critical Zone: START ---
    //
    pthread_rwlock_wrlock(&rw_lock);
    try {
        auto it = map->find(key);

        if (it != map->end())
            it->second = it->second + value;

        else
            map->insert(std::make_pair(key, value));

        pthread_rwlock_unlock(&rw_lock);
        return 0;
    }
    catch(std::exception e){
        pthread_rwlock_unlock(&rw_lock);
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
template <typename K, typename V> int Thread_Safe_KV_Store_2<K, V>::lookup(const K key, V& value){

    //
    // ---- Critical Zone: START ---
    //
    pthread_rwlock_rdlock(&rw_lock);
    bool found = false;
    for(auto it : *map){
        if(it.first == key){
            value = it.second;
            found = true;
            break;
        }
    }
    pthread_rwlock_unlock(&rw_lock);
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
template <typename K, typename V> int Thread_Safe_KV_Store_2<K, V>::remove(const K key){

    pthread_rwlock_wrlock(&rw_lock);
    try{
        map->erase(key);
        pthread_rwlock_unlock(&rw_lock);
        return 0;
    }
    catch(std::exception e){
        pthread_rwlock_unlock(&rw_lock);
        return -1;
    }

}

/*
 * This method checks whether the unordered map already contains the value and returns value of type boolean.
 */
template<typename K, typename V> bool Thread_Safe_KV_Store_2<K, V>::contains(const K key) {

    pthread_rwlock_rdlock(&rw_lock);
    for( auto it : *map){
        if(it.first == key){
            pthread_rwlock_unlock(&rw_lock);
            return true;
        }
    }
    pthread_rwlock_unlock(&rw_lock);
    return false;
}

/*
 * Returns the current size of the KV Store
 */
template  <typename K, typename V> int Thread_Safe_KV_Store_2<K, V>::size(){
    pthread_rwlock_rdlock(&rw_lock);
    long temp = map->size();
    pthread_rwlock_unlock(&rw_lock);
    return temp;
};


template <typename K, typename V> int Thread_Safe_KV_Store_2<K, V>::remove_random() {
    try {
        pthread_rwlock_rdlock(&rw_lock);
        srand(time(NULL));
        int r = rand() % map->size();
        auto it = map->begin();
        for ( int i=0; i<r && i<map->size(); i++) { it++; }
        if (map->size()) { map->erase(it); }
        pthread_rwlock_unlock(&rw_lock);
        return 0;
    }
    catch (std::exception e) {
        pthread_rwlock_unlock(&rw_lock);
        return -1;
    }
};

//The following methods are Not thread safe. Beware

template <typename K, typename V> typename std::unordered_map<K, V>::iterator Thread_Safe_KV_Store_2<K, V>::begin(){
    return map->begin();
};

template <typename K, typename V> typename std::unordered_map<K, V>::iterator Thread_Safe_KV_Store_2<K, V>::end(){
    return map->end();
};


template class Thread_Safe_KV_Store_2<std::string, int>;
template class Thread_Safe_KV_Store_2<const char*, int>;
template class Thread_Safe_KV_Store_2<char*, int>;
template class Thread_Safe_KV_Store_2<char, int>;
template class Thread_Safe_KV_Store_2<int, int>;

template class Thread_Safe_KV_Store_2<std::string, std::string>;
//template class Thread_Safe_KV_Store_2<std::string, const char*>;
//template class Thread_Safe_KV_Store_2<std::string, char*>;
//template class Thread_Safe_KV_Store_2<std::string, char>;

