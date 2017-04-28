//
// Created by James on 3/28/17.
//


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <unordered_set>

#include "Thread_Safe_KV_Store_2.h"
#include "Thread_Safe_KV_Store_2.cpp"



class sample{

public:
    sample(){

    };
    void add(int a){
        map.insert(a);
    };
    bool get(int a){
        return true;
    }

private:
    std::unordered_set<int> map;

};


int main(){


//    sample* t = new sample();
//    t->add(5);

    Thread_Safe_KV_Store_2<int, int>* test = new Thread_Safe_KV_Store_2<int, int>::Thread_Safe_KV_Store_2<int, int>();
    printf("Created an instance\n");


    test->insert(100, 0);
    printf("Inserted a value\n");
    printf("Size is %d\n", test->size());

    test->insert(101, 0);
    printf("Inserted a value\n");
    printf("Size is %d\n", test->size());

    test->insert(102, 0);
    printf("Inserted a value\n");
    printf("Size is %d\n", test->size());

    test->insert(103, 0);
    printf("Inserted a value\n");
    printf("Size is %d\n", test->size());


    printf("Removing random value\n");
    test->remove_random();
    printf("Size is %d\n", test->size());

    printf("Removing random value\n");
    test->remove_random();
    printf("Size is %d\n", test->size());

    printf("Removing random value\n");
    test->remove_random();
    printf("Size is %d\n", test->size());

    printf("Removing random value\n");
    test->remove_random();
    printf("Size is %d\n", test->size());

    return 0;
}

