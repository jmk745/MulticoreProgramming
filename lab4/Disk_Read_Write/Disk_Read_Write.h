//
// Created by James on 4/28/17.
//

#ifndef LAB4_DISK_READ_WRITE_H
#define LAB4_DISK_READ_WRITE_H
//
// Created by James on 4/25/17.
//

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <cstring>
#include <sys/stat.h>
#include <exception>

#include "../Thread_Safe_KV_Store_2/Thread_Safe_KV_Store_2.h"

//Basic Functions for reading and writing from files on disk

int write_to_file (const char* key, int value, const char* hash, pthread_mutex_t* mutex, pthread_cond_t* condition, pthread_mutex_t* cond_mutex);

int read_from_file (const char* key, int* value, char* hash, pthread_mutex_t* mutex, pthread_cond_t* condition, pthread_mutex_t* cond_mutex);

int delete_from_file(const char* key, pthread_mutex_t* mutex, pthread_cond_t* condition, pthread_mutex_t* cond_mutex);



//Upgraded functions to inlcude KV_store in one atomic read or write

int write_to_file_and_cache (const char* key, int value, const char* hash, Thread_Safe_KV_Store_2<std::string, int>* kv_store, Thread_Safe_KV_Store_2<std::string, std::string>* hash_store,
                             pthread_mutex_t* mutex, pthread_cond_t* condition, pthread_mutex_t* cond_mutex);

int read_from_file_and_cache (const char* key, int* value, char* hash, Thread_Safe_KV_Store_2<std::string, int>* kv_store, Thread_Safe_KV_Store_2<std::string, std::string>* hash_store,
                              pthread_mutex_t* mutex, pthread_cond_t* condition, pthread_mutex_t* cond_mutex);

int delete_from_file_and_cache (const char* key, Thread_Safe_KV_Store_2<std::string, int>* kv_store, Thread_Safe_KV_Store_2<std::string, std::string>* hash_store,
                               pthread_mutex_t* mutex, pthread_cond_t* condition, pthread_mutex_t* cond_mutex);

///*
//a more efficient implementation where a value is written/updated to disk only if it is evicted.
int smart_write_to_file_and_cache (const char* key, int value, const char* hash, Thread_Safe_KV_Store_2<std::string, int>* kv_store,
                                   Thread_Safe_KV_Store_2<std::string, std::string>* hash_Store, pthread_mutex_t* mutex, pthread_cond_t* condition, pthread_mutex_t* cond_mutex);


#endif //LAB4_DISK_READ_WRITE_H
