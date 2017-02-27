#declarations
CC=g++
CFLAGS=-c

jk_lab0: main.o set.o
	$(CC) main.o set.o -o jk_lab0;

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

set.o: set.cpp
		$(CC) $(CFLAGS) set.cpp

clean:
	rm *.o jk_lab0
