CC=g++
cFlag=-c -pthread

test.out: test_for_Thread_Safe_Queue.o Thread_Safe_Queue.o
	${CC} test_for_Thread_Safe_Queue.o Thread_Safe_Queue.o -o test.out

test_for_Thread_Safe_Queue.o: test_for_Thread_Safe_Queue.cpp
	${CC} ${cFlag} test_for_Thread_Safe_Queue.cpp -o test_for_Thread_Safe_Queue.o

Thread_Safe_Queue.o: Thread_Safe_Queue.cpp Thread_Safe_Queue.h
	${CC} ${cFlag} Thread_Safe_Queue.cpp -o Thread_Safe_Queue.o

clean:
	rm *.o test.out
