CC=g++
cFlag=-c

test.out: main.o Thread_Safe_KV_Store_2.o Disk_Read_Write.o
	${CC} main.o Thread_Safe_KV_Store_2.o Disk_Read_Write.o -o test.out

main.o: main.cpp
	${CC} ${cFlag} main.cpp -o main.o

Thread_Safe_KV_Store_2.o: ../Thread_Safe_KV_Store_2/Thread_Safe_KV_Store_2.cpp
	${CC} ${cFlag} ../Thread_Safe_KV_Store_2/Thread_Safe_KV_Store_2.cpp -o Thread_Safe_KV_Store_2.o

Disk_Read_Write.o: Disk_Read_Write.cpp
	${CC} ${cFlag} Disk_Read_Write.cpp -o Disk_Read_Write.o

clean:
	rm *.out *.o
