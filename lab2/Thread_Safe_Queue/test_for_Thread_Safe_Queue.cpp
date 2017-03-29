//
// Created by James on 3/28/17.
//


#include <unordered_map>
#include <unistd.h>

#include "Thread_Safe_Queue.h"
#include "Thread_Safe_Queue.cpp"

typedef struct{
    std::unordered_map<int, int>* a;
    int b;
    int c;
} cont;




typedef struct{
    void (*fn)(void*);
    cont* data;
} cont2;




void test_method_1(void* obj_ptr){
    cont* temp_cont = (cont*) obj_ptr;
    std::unordered_map<int, int>* temp_map =  (std::unordered_map<int, int>*) temp_cont->a;
    temp_map->insert( std::make_pair(temp_cont->b, temp_cont->c) );
    printf("Inserted Element\n");
}


void* thread_function1(void* v){
    printf("Thread %ld\n", pthread_self());
    Thread_Safe_Queue<int>* q = (Thread_Safe_Queue<int>*) v;

    for(int i=0; i<100; i++){
        q->enqueue(i);
    }
    pthread_exit(0);
}

void* thread_function2(void* v){
    printf("Thread %ld\n", pthread_self());
    Thread_Safe_Queue<int>* q = (Thread_Safe_Queue<int>*) v;

    for(int i=0; i<100; i++){
        int x = q->dequeue();
        printf("%d\n", x);
    }

    pthread_exit(0);
}




int main(){

//    Thread_Safe_Queue<cont*>* test = new Thread_Safe_Queue<cont*>();
//
//    Thread_Safe_Queue<cont2*>* test2 = new Thread_Safe_Queue<cont2*>();
//
//    std::unordered_map<int, int> map;
//
//
//
//    for (int i=0; i<10; i++){
//
//        cont* abc = new cont();
//        abc->a = &map;
//        abc->b = i;
//        abc->c = i*100;
//        printf("%d %d\n", i, i*100);
//        test->enqueue(abc);
//
//
//
//        cont2* def = new cont2();
//        def->fn = test_method_1;
//        def->data = abc;
//        test2->enqueue(def);
//    }
//
//    printf("-----------------\n");
//
//
//    while ( !test->isEmpty() ){
//        cont* temp = test->dequeue();
//        int temp_a = temp->b;
//        int temp_b = temp->c;
//        printf("%d %d\n", temp_a, temp_b);
//
//        cont2* temp2 = test2->dequeue();
//
//        temp2->fn(temp2->data);
//    }
//
//
//    auto it = map.begin();
//
//    while(it != map.end()){
//        printf("%d  :   %d\n", it->first, it->second);
//        it++;
//    }



    printf("\n\n--- Part2 --- \n\n");


    Thread_Safe_Queue<int>* test = new Thread_Safe_Queue<int>();


    int number_of_threads=2;

    pthread_t tid1;
    pthread_t tid2;
    pthread_create(&tid1, NULL, thread_function1, (void*) test);
//    usleep(2000000);
    pthread_create(&tid2, NULL, thread_function2, (void*) test);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    printf("\n\nTest Completed\n\n");

    return 0;
}





