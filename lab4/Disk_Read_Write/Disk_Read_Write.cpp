//
// Created by James on 4/25/17.
//

#include "Disk_Read_Write.h"
#include "../Thread_Safe_KV_Store_2/Thread_Safe_KV_Store_2.h"
#include "../md5/md5.h"

// R/W/D functions for server without a cache memory store

int write_to_file (const char* key, int value, const char* hash, pthread_mutex_t* mutex, pthread_cond_t* condition, pthread_mutex_t* cond_mutex) {

    char key_wrlock[50];
    char key_rdlock[50];
    strcpy(key_wrlock, key);
    strcpy(key_rdlock , key);
    strcat(key_wrlock, ".wrlock");
    strcat(key_rdlock, ".rdlock");
    FILE* file;

    while (1) {

        //
        // Critical Zone -- Start
        //
        pthread_mutex_lock(&(*mutex));
        if ( (access(key_wrlock, F_OK) == -1 ) && access(key_rdlock, F_OK) == -1) { //check if not writer or reader locked
            file = fopen(key_wrlock, "w"); //create a write lock
            fsync(fileno(file)); //ensure that all is written to disk
            fclose(file);
            pthread_mutex_unlock(&(*mutex));
            break; //exit loop and proceed to writing to the file
        }
        pthread_mutex_unlock(&(*mutex));
        //
        // Critical Zone -- End
        //

        pthread_mutex_lock(&(*cond_mutex));
        pthread_cond_wait(&(*condition), &(*cond_mutex)); //WAIT until further notice
        pthread_mutex_unlock(&(*cond_mutex));
    }

    //Delete file if it exists in order to replace with new value
    if (access(key, F_OK) != -1) {
        remove(key);
    }
    file = fopen(key, "w");
    fprintf(file, "%i\n", value); //write to the file
    fprintf(file, "%s", hash); //write to the file
    fsync(fileno(file)); //ensure that all is written to disk
    fclose(file);

    //
    // Critical Zone -- Start
    //
    pthread_mutex_lock(&(*mutex)); //remove the lock and exit
    remove(key_wrlock);
    pthread_mutex_unlock(&(*mutex));
    //
    // Critical Zone -- End
    //
    pthread_cond_broadcast(&(*condition)); //signal to other threads that lock state ahs changed
    return 0;
}

int read_from_file (const char* key, int* value, char* hash, pthread_mutex_t* mutex, pthread_cond_t* condition, pthread_mutex_t* cond_mutex) {

    char key_wrlock[50];
    char key_rdlock[50];
    strcpy(key_wrlock, key);
    strcpy(key_rdlock , key);
    strcat(key_wrlock, ".wrlock");
    strcat(key_rdlock, ".rdlock");
    FILE* file;

    while (1) {
        //
        // Critical Zone -- Start
        //
        pthread_mutex_lock(&(*mutex));
        if (access(key_wrlock, F_OK) == -1) { //if not write locked, then proceed
            file = fopen(key_rdlock, "w");  //Create the rd_lock on disk
            fsync(fileno(file)); //ensure that all is written to disk
            fclose(file);
            pthread_mutex_unlock(&(*mutex));
            break; //leave the loop after creating the lock file and proceed to reading from file
        }
        pthread_mutex_unlock(&(*mutex));
        //
        // Critical Zone -- End
        //

        //wait until next signal and check if the it will be write locked
        pthread_mutex_lock(&(*cond_mutex));
        pthread_cond_wait(&(*condition), &(*cond_mutex)); //WAIT until further notice
        pthread_mutex_unlock(&(*cond_mutex));
    }

    //Read data off the file
    file = fopen(key, "r");
    if (file) {
        fscanf(file, "%i", &(*value));
        fscanf(file, "%s", hash);
        fsync(fileno(file)); //ensure that all is written to disk
        fclose(file);
    }

    //
    // Critical Zone -- Start
    //
    pthread_mutex_lock(&(*mutex));
    remove(key_rdlock);       //Delete the RD lock on disk
    pthread_mutex_unlock(&(*mutex));
    //
    // Critical Zone -- End
    //
    pthread_cond_broadcast(&(*condition)); //signal to other threads that lock state has changed
    return 0;
}

int delete_from_file(const char* key, pthread_mutex_t* mutex, pthread_cond_t* condition, pthread_mutex_t* cond_mutex){

    char key_wrlock[50];
    char key_rdlock[50];
    strcpy(key_wrlock, key);
    strcpy(key_rdlock , key);
    strcat(key_wrlock, ".wrlock");
    strcat(key_rdlock, ".rdlock");
    FILE* file;

    while (1) {

        //
        // Critical Zone -- Start
        //
        pthread_mutex_lock(&(*mutex));
        if ( (access(key_wrlock, F_OK) == -1 ) && access(key_rdlock, F_OK) == -1) { //check if not writer or reader locked
            file = fopen(key_wrlock, "w"); //create a write lock
            fsync(fileno(file)); //ensure that all is written to disk
            fclose(file);
            pthread_mutex_unlock(&(*mutex));
            break; //exit loop and proceed to writing to the file
        }
        pthread_mutex_unlock(&(*mutex));
        //
        // Critical Zone -- End
        //

        pthread_mutex_lock(&(*cond_mutex));
        pthread_cond_wait(&(*condition), &(*cond_mutex)); //WAIT until further notice
        pthread_mutex_unlock(&(*cond_mutex));
    }

    int found = -1;
    //Delete file if it exists
    if (access(key, F_OK) != -1) {
        remove(key);
        found = 0;
    }
    //
    // Critical Zone -- Start
    //
    pthread_mutex_lock(&(*mutex)); //remove the lock and exit
    remove(key_wrlock);
    pthread_mutex_unlock(&(*mutex));
    //
    // Critical Zone -- End
    //
    pthread_cond_broadcast(&(*condition)); //signal to other threads that lock state ahs changed
    return found;
}





// R/W/D functions for server with a cache memory store

int write_to_file_and_cache (const char* key, int value, const char* hash, Thread_Safe_KV_Store_2<std::string, int>* kv_store, Thread_Safe_KV_Store_2<std::string, std::string>* hash_store,
                             pthread_mutex_t* mutex, pthread_cond_t* condition, pthread_mutex_t* cond_mutex) {

    char key_wrlock[50];
    char key_rdlock[50];
    strcpy(key_wrlock, key);
    strcpy(key_rdlock , key);
    strcat(key_wrlock, ".wrlock");
    strcat(key_rdlock, ".rdlock");
    FILE* file;

    while (1) {

        //
        // Critical Zone -- Start
        //
        pthread_mutex_lock(&(*mutex));
        if ( (access(key_wrlock, F_OK) == -1 ) && access(key_rdlock, F_OK) == -1) { //check if not writer or reader locked
            file = fopen(key_wrlock, "w"); //create a write lock
            fsync(fileno(file)); //ensure that all is written to disk
            fclose(file);
            //if kv store is at max size, then remove an item at random and insert the new key
            if (kv_store->size() >= 128) {
                auto pair = kv_store->get_random();
                kv_store->remove(pair->first);
                hash_store->remove(pair->first);
            }
            kv_store->insert(key, value);
            hash_store->insert(key, hash);
            pthread_mutex_unlock(&(*mutex));
            break; //exit loop and proceed to writing to the file
        }
        pthread_mutex_unlock(&(*mutex));
        //
        // Critical Zone -- End
        //

        pthread_mutex_lock(&(*cond_mutex));
        pthread_cond_wait(&(*condition), &(*cond_mutex)); //WAIT until further notice
        pthread_mutex_unlock(&(*cond_mutex));
    }

    //Delete file if it exists in order to replace with new value
    if (access(key, F_OK) != -1) {
        remove(key);
    }
    file = fopen(key, "w");
    fprintf(file, "%i\n", value); //write value to the file
    fprintf(file, "%s\n", hash); //write hash to the file
    fsync(fileno(file)); //ensure that all is written to disk
    fclose(file);
    //
    // Critical Zone -- Start
    //
    pthread_mutex_lock(&(*mutex)); //remove the lock and exit
    remove(key_wrlock);
    pthread_mutex_unlock(&(*mutex));
    //
    // Critical Zone -- End
    //
    pthread_cond_broadcast(&(*condition)); //signal to other threads that lock state ahs changed
    return 0;
}

int read_from_file_and_cache (const char* key, int* value, char* hash, Thread_Safe_KV_Store_2<std::string, int>* kv_store, Thread_Safe_KV_Store_2<std::string, std::string>* hash_store,
                              pthread_mutex_t* mutex, pthread_cond_t* condition, pthread_mutex_t* cond_mutex) {

    char key_wrlock[50];
    char key_rdlock[50];
    strcpy(key_wrlock, key);
    strcpy(key_rdlock , key);
    strcat(key_wrlock, ".wrlock");
    strcat(key_rdlock, ".rdlock");
    FILE* file;

    while (1) {
        //
        // Critical Zone -- Start
        //
        pthread_mutex_lock(&(*mutex));

        if (kv_store->lookup(key, *value) == 0) {
            //the followjg embeded lines are need to convert from string to char* in ordered to return the hash
            std::string temp_string;
            hash_store->lookup(key, temp_string);
            std::vector<char> temp_vector(temp_string.length() + 1);
            std::strcpy(hash, temp_string.c_str());
            pthread_mutex_unlock(&(*mutex));        /// Critical Zone End !!!!!! leaving function
            return 0; //item is found
        }
            //proceed to check into files
        else if (access(key_wrlock, F_OK) == -1) { //if not write locked, then proceed
            file = fopen(key_rdlock, "w");  //Create the rd_lock on disk
            fsync(fileno(file)); //ensure that all is written to disk
            fclose(file);
            pthread_mutex_unlock(&(*mutex));
            break; //leave the loop after creating the lock file and proceed to reading from file
        }
        pthread_mutex_unlock(&(*mutex));
        //
        // Critical Zone -- End
        //

        //wait until next signal and check if the it will be write locked
        pthread_mutex_lock(&(*cond_mutex));
        pthread_cond_wait(&(*condition), &(*cond_mutex)); //WAIT until further notice
        pthread_mutex_unlock(&(*cond_mutex));
    }

    //Assume not found unless, file is opened to obtain value
    int is_found = -1;

    //Read data off the file
    file = fopen(key, "r");
    if (file) {
        fscanf(file, "%i", &(*value));
        fscanf(file, "%s", hash);
        fsync(fileno(file)); //ensure that all changes is written to disk
        fclose(file);
        is_found = 0; //mark as found
    }

    //
    // Critical Zone -- Start
    //
    pthread_mutex_lock(&(*mutex));
    remove(key_rdlock);       //Delete the RD lock on disk
    pthread_mutex_unlock(&(*mutex));
    //
    // Critical Zone -- End
    //
    pthread_cond_broadcast(&(*condition)); //signal to other threads that lock state has changed
    return is_found;
}

int delete_from_file_and_cache(const char* key, Thread_Safe_KV_Store_2<std::string, int>* kv_store, Thread_Safe_KV_Store_2<std::string, std::string>* hash_store,
                               pthread_mutex_t* mutex, pthread_cond_t* condition, pthread_mutex_t* cond_mutex){

    char key_wrlock[50];
    char key_rdlock[50];
    strcpy(key_wrlock, key);
    strcpy(key_rdlock , key);
    strcat(key_wrlock, ".wrlock");
    strcat(key_rdlock, ".rdlock");
    FILE* file;

    while (1) {
        //
        // Critical Zone -- Start
        //
        pthread_mutex_lock(&(*mutex));
        if ( (access(key_wrlock, F_OK) == -1 ) && access(key_rdlock, F_OK) == -1) { //check if not writer or reader locked
            file = fopen(key_wrlock, "w"); //create a write lock
            fsync(fileno(file)); //ensure that all is written to disk
            fclose(file);
            kv_store->remove(key); //delete from kv store and proceed onto files
            hash_store->remove(key); // delete from the hash store
            pthread_mutex_unlock(&(*mutex));
            break; //exit loop and proceed to writing to the file
        }
        pthread_mutex_unlock(&(*mutex));
        //
        // Critical Zone -- End
        //

        pthread_mutex_lock(&(*cond_mutex));
        pthread_cond_wait(&(*condition), &(*cond_mutex)); //WAIT until further notice
        pthread_mutex_unlock(&(*cond_mutex));
    }

    int found = -1;
    //Delete file if it exists
    if (access(key, F_OK) != -1) {
        remove(key);
        found = 0;
    }
    //
    // Critical Zone -- Start
    //
    pthread_mutex_lock(&(*mutex)); //remove the lock and exit
    remove(key_wrlock);
    pthread_mutex_unlock(&(*mutex));
    //
    // Critical Zone -- End
    //
    pthread_cond_broadcast(&(*condition)); //signal to other threads that lock state ahs changed
    return found;
}


//Smarter implementation of write.. Instead of write through, we do write back
int smart_write_to_file_and_cache (const char* key, int value, const char* hash, Thread_Safe_KV_Store_2<std::string, int>* kv_store, Thread_Safe_KV_Store_2<std::string, std::string>* hash_store,
                             pthread_mutex_t* mutex, pthread_cond_t* condition, pthread_mutex_t* cond_mutex) {

    char key_wrlock[50];
    char key_rdlock[50];
    FILE* file;
    auto pair = kv_store->get_random(); //get random to define the type for auto
    std::string pair_key, pair_hash;
    int pair_value;
    while (1) {

        //
        // Critical Zone -- Start
        //
        pthread_mutex_lock(&(*mutex));
        pair = kv_store->get_random(); // get a random pair from kv_store
        strcpy(key_wrlock, (pair) ? pair->first.c_str():"");
        strcpy(key_rdlock , (pair) ? pair->first.c_str():"");
        strcat(key_wrlock, ".wrlock");
        strcat(key_rdlock, ".rdlock");

        if (kv_store->lookup(key, value) == 0 || kv_store->size() < 128) { //if it exists of cache store is not full, then insert/update
            kv_store->insert(key, value);
            hash_store->insert(key, hash);
            pthread_mutex_unlock(&(*mutex));
            return 0; //exit right after since the method only writes to disk after a pair is evicted from the unordered cache map
        }
        else if ( (access(key_wrlock, F_OK) == -1 ) && access(key_rdlock, F_OK) == -1) { //check if not writer or reader locked
            file = fopen(key_wrlock, "w"); //create a write lock
            fsync(fileno(file)); //ensure that all is written to disk
            fclose(file);
            pair_key = pair->first;
            kv_store->lookup(pair_key, pair_value);
            hash_store->lookup(pair->first, pair_hash); // retrieve the hash value

            kv_store->remove(pair->first); //remove the selected random pair and then..
            hash_store->remove(pair->first); //remove hash from store as well and finally..

            kv_store->insert(key, value); // insert our new one
            hash_store->insert(key, hash);

            pthread_mutex_unlock(&(*mutex));
            break; //exit loop and proceed to writing to the file
        }
        pthread_mutex_unlock(&(*mutex));
        //
        // Critical Zone -- End
        //

        pthread_mutex_lock(&(*cond_mutex));
        pthread_cond_wait(&(*condition), &(*cond_mutex)); //WAIT until further notice
        pthread_mutex_unlock(&(*cond_mutex));
    }

    printf("C\n");
    //Delete data file if it exists in order to replace with new value
    if (access(pair_key.c_str(), F_OK) != -1) {
        remove(key);
    }
    printf("D\n");
    file = fopen(pair_key.c_str(), "w"); //create and..
    printf("E\n");
    fprintf(file, "%i\n", pair_value); //write the value of the evicted pair to the file
    printf("F\n");
    fprintf(file, "%s", pair_hash.c_str());
    fsync(fileno(file)); //ensure that all is written to disk
    fclose(file);
    //
    // Critical Zone -- Start
    //
    pthread_mutex_lock(&(*mutex)); //remove the lock and exit
    remove(key_wrlock);
    pthread_mutex_unlock(&(*mutex));
    //
    // Critical Zone -- End
    //
    pthread_cond_broadcast(&(*condition)); //signal to other threads that lock state ahs changed
    return 0;
}
