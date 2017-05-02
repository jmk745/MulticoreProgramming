//
// Created by James on 3/28/17.
//


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <unordered_set>

#include "Thread_Safe_KV_Store_2.h"
//#include "Thread_Safe_KV_Store_2.cpp"



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


    Thread_Safe_KV_Store_2<int, int>* test = new Thread_Safe_KV_Store_2<int, int>::Thread_Safe_KV_Store_2<int, int>();
    printf("Created an instance\n");


    test->insert(4, 21);
    printf("Inserted a value\n");
    printf("Size is %d\n", test->size());

    int x;
    test->lookup(4, x);
    printf("Value for 0 is: %d\n", x);

    std::pair<int, int>* p = test->remove_random_and_return();
    printf("Obtained pair with first: %i and second: %i\n", p->first, p->second);
    printf("Size is %d\n", test->size());
    printf("Test Completed\n\n");
    return 0;
}

