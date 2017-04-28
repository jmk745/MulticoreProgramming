//
// Created by James on 4/25/17.
//

#include "Disk_Read_Write.h"


int write_to_file (const char* key, int value, pthread_mutex_t* mutex, pthread_cond_t* condition, pthread_mutex_t* cond_mutex) {

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
    fprintf(file, "%i", value); //write to the file
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

int read_from_file (const char* key, int* value, pthread_mutex_t* mutex, pthread_cond_t* condition, pthread_mutex_t* cond_mutex) {

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








int write_to_file_and_cache (const char* key, int value, Thread_Safe_KV_Store_2<std::string, int>* kv_store,
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
            fclose(file);
            //if kv store is at max size, then remove an item at random and insert the new key
            if (kv_store->size() >= 128) {
                kv_store->remove_random();
            }
            kv_store->insert(key, value);
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
    fprintf(file, "%i", value); //write to the file
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

int read_from_file_and_cache (const char* key, int* value, Thread_Safe_KV_Store_2<std::string, int>* kv_store,
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
            return 0; //item is found
        }
            //proceed to check into files
        else if (access(key_wrlock, F_OK) == -1) { //if not write locked, then proceed
            file = fopen(key_rdlock, "w");  //Create the rd_lock on disk
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

int delete_from_file_and_cache(const char* key, Thread_Safe_KV_Store_2<std::string, int>* kv_store,
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
            fclose(file);
            kv_store->remove(key); //delete from kv store and proceed onto files
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