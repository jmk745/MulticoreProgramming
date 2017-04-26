CC=g++
cFlag=-c -pthread -std=c++11

test.out: test_for_Thread_Pool.o Thread_Pool.o
	${CC} -std=c++14 Thread_Pool.o test_for_Thread_Pool.o -o test.out

test_for_Thread_Pool.o: test_for_Thread_Pool.cpp
	${CC} ${cFlag} test_for_Thread_Pool.cpp

Thread_Pool.o: Thread_Pool.cpp Thread_Pool.h
	${CC} ${cFlag} Thread_Pool.cpp -o Thread_Pool.o

clean:
	rm *.o test.out
