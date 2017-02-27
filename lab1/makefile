CC = g++
flags = -c -std=c++11 -pthread

lab1: main.o ThreadSafeKVStore.o ThreadSafeListenerQueue.o testThread.o
	${CC} main.o ThreadSafeKVStore.o ThreadSafeListenerQueue.o -o lab1

main.o: main.cpp
	${CC} ${flags} main.cpp -o main.o

ThreadSafeKVStore.o: ThreadSafeKVStore.cpp ThreadSafeKVStore.h
	${CC} ${flags} ThreadSafeKVStore.cpp -o ThreadSafeKVStore.o

ThreadSafeListenerQueue.o: ThreadSafeListenerQueue.cpp ThreadSafeListenerQueue.h
	${CC} ${flags} ThreadSafeListenerQueue.cpp -o ThreadSafeListenerQueue.o

testThread.o: testThread.cpp
	${CC} ${flags} testThread.cpp -o testThread.o

clean:
	rm *.o lab1
