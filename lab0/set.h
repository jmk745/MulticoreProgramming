
#ifndef SET_H_
#define SET_H_

#include <list>
//#include "set.cpp"

template <class T> class Set{

  private:
      std::list<T> _setList;

  public:
    int insert(T key);
    bool find(T key);
    int remove(T key);
    void print();
    bool isEmpty();
};

#endif //SET_H_
