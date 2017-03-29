CC=g++
cFlag=-g -c -std=c++11 -pthread

test.out: test.o Thread_Safe_KV_Store_2.o
	${CC} -g test.o Thread_Safe_KV_Store_2.o -o test.out

test.o: test_for_Thread_Safe_KV_Store_2.cpp Thread_Safe_KV_Store_2.cpp Thread_Safe_KV_Store_2.h
	${CC} ${cFlag} test_for_Thread_Safe_KV_Store_2.cpp -o test.o

Thread_Safe_KV_Store_2.o: Thread_Safe_KV_Store_2.cpp Thread_Safe_KV_Store_2.h
	${CC} ${cFlag} Thread_Safe_KV_Store_2.cpp -o Thread_Safe_KV_Store_2.o

clean:
	rm test.out *.o
