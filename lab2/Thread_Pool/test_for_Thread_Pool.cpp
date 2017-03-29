//
// Created by James on 3/28/17.
//


#include <unistd.h>

#include "Thread_Pool.h"


typedef struct{
    int num;
} container;


void task_work(void* data){

    int x = *((int*) data);

    printf("%ld : %d \nDoing Work..\n", pthread_self(), x);
    for(int i=0; i<999999; i++){}
}


int main(){

    Thread_Pool* thread_pool = new Thread_Pool();

    thread_pool->init(2);

    usleep(1000000);

    for(int i=0; i<1; i++) {
        container *cont = new container();
        cont->num = 5;
        thread_pool->add_task(task_work, cont);
    }

    delete thread_pool;

    return 0;
}