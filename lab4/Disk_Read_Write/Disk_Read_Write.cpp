//
// Created by James on 4/25/17.
//

#include "Disk_Read_Write.h"

//#include "Thread_Safe_KV_Store_2/Thread_Safe_KV_Store_2.h"
//#include "Thread_Safe_KV_Store_2/Thread_Safe_KV_Store_2.cpp"

//
//int write_to_file(const char* key, int value, pthread_rwlock_t* rw_lock) {
//
//    try {
//        pthread_rwlock_wrlock(&(*rw_lock));
//        // if file exits -> then delete it
//        if (access(key, F_OK) != -1) {
//            remove(key);
//        }
//        FILE* file = fopen(key, "w");
//        fprintf(file, "%i", value);
//        fclose(file);
//        pthread_rwlock_unlock(&(*rw_lock));
//        return 0;
//    }
//    catch (std::exception e) {
//        pthread_rwlock_unlock(&(*rw_lock));
//        return -1;
//    }
//}


//int read_from_file(const char* key, int* value, pthread_rwlock_t* rw_lock) {
//
//    try {
//        pthread_rwlock_wrlock(&(*rw_lock));
//        FILE* file = fopen(key, "r");
//        if (file) {
//            fscanf(file, "%i", &(*value));
//            fclose(file);
//        }
//        pthread_rwlock_unlock(&(*rw_lock));
//        return 0;
//    }
//    catch (std::exception e) {
//        pthread_rwlock_unlock(&(*rw_lock));
//        return -1;
//    }
//}

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