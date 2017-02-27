#include <stdlib.h>
#include <stdio.h>
#include "set.h"


// This method invokes the iterator for the list and checks each value against the element
// in question.
template <typename T> bool Set<T>::find(T const key){
  for( typename std::list<T>::iterator it = this->_setList.begin(); it != this->_setList.end(); it++){
    if( *it == key){
      return true;
    }
  }
  return false;
}

// The Set insert method attempts to first find the element in the list.
// If it is not found, then the element is added. Otherwise, the method terminates
// without the insertion.
template <typename T> int Set<T>::insert(T key){
  try{
    if( find(key) != true ){
        this->_setList.push_front(key);
    }
    return 0;
  }
  catch(int e){}

  return -1;
}


// This method removes the element from the list if it is present.
// Else no action is performed.
template <typename T> int Set<T>::remove(T key){
  try{
      this->_setList.remove(key);
      return 0;
  }
  catch(int e){}

  return -1;
}

// Used to display entire Set to the user in the terminal.
template <typename T> void Set<T>::print(){
  for( typename std::list<T>::iterator it = this->_setList.begin(); it != this->_setList.end() ; it++ ){
    printf("%d, ", *it);
  }
  printf("\n");
}

// Returns whether the Set DS is currently empty.
template <typename T> bool Set<T>::isEmpty(){
  return _setList.empty();
}


//Explicit Instatiations
template class Set<int>;
template class Set<char>;
