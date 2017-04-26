//
// Created by James on 4/25/17.
//

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <exception>


int write_to_file(const char* key, int value, pthread_rwlock_t* rw_lock) {

    try {
        pthread_rwlock_wrlock(&(*rw_lock));
        // if file exits -> then delete it
        if (access(key, F_OK) != -1) {
            remove(key);
        }
        FILE* file = fopen(key, "w");
        fprintf(file, "%i", value);
        fclose(file);
        pthread_rwlock_unlock(&(*rw_lock));
        return 0;
    }
    catch (std::exception e) {
        pthread_rwlock_unlock(&(*rw_lock));
        return -1;
    }

}


int read_from_file(const char* key, int* value, pthread_rwlock_t* rw_lock) {

    try {
        pthread_rwlock_wrlock(&(*rw_lock));
        FILE* file = fopen(key, "r");
        if (file) {
            fscanf(file, "%i", &(*value));
            fclose(file);
        }
        pthread_rwlock_unlock(&(*rw_lock));
        return 0;
    }
    catch (std::exception e) {
        pthread_rwlock_unlock(&(*rw_lock));
        return -1;
    }
}


int main (int argc, char** argv) {
    pthread_rwlock_t rw_lock;
    pthread_rwlock_init(&rw_lock, NULL);

    write_to_file("data/JAMES", 12345, &rw_lock);

    int i;
    read_from_file("data/JAMES", &i, &rw_lock);

    printf("%i\n", i);
    printf("DONE!\n");

    return 0;
}
