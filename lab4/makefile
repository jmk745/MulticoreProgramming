CC=g++
cFlag=-c -std=c++0x -pthread

#Regular Server from lab2
server.out: Threaded_Server.o Thread_Safe_KV_Store_2.o Thread_Pool.o httpreq.o httpresp.o
	${CC} Threaded_Server.o Thread_Safe_KV_Store_2.o Thread_Pool.o httpreq.o httpresp.o -o server.out -pthread

#server with global lock
server_g.out: Threaded_Server_G.o Thread_Safe_KV_Store.o Thread_Pool.o httpreq.o httpresp.o
	${CC} Threaded_Server_G.o Thread_Safe_KV_Store.o Thread_Pool.o httpreq.o httpresp.o -o server_g.out -pthread

#server with read write to cache store and disk
server_rw.out: Threaded_Server_RW.o Thread_Safe_KV_Store_2.o Thread_Pool.o httpreq.o httpresp.o Disk_Read_Write.o
	${CC} Threaded_Server_RW.o Thread_Pool.o Thread_Safe_KV_Store_2.o httpreq.o httpresp.o Disk_Read_Write.o -o server_rw.out -pthread

#server with only disk writes
server_disk.out: Threaded_Server_Disk.o Thread_Safe_KV_Store_2.o Thread_Pool.o httpreq.o httpresp.o Disk_Read_Write.o
	${CC} Threaded_Server_Disk.o Thread_Pool.o Thread_Safe_KV_Store_2.o httpreq.o httpresp.o Disk_Read_Write.o -o server_disk.out -pthread

#-----------------------------------------------
#Object Files to create the excecutables
Thread_Pool.o: Thread_Pool/Thread_Pool.cpp Thread_Pool/Thread_Pool.h
	${CC} ${cFlag} Thread_Pool/Thread_Pool.cpp -o Thread_Pool.o

Thread_Safe_KV_Store.o: Thread_Safe_KV_Store/ThreadSafeKVStore.cpp Thread_Safe_KV_Store/ThreadSafeKVStore.h
	${CC} ${cFlag} Thread_Safe_KV_Store/ThreadSafeKVStore.cpp -o Thread_Safe_KV_Store.o

Thread_Safe_KV_Store_2.o: Thread_Safe_KV_Store_2/Thread_Safe_KV_Store_2.cpp Thread_Safe_KV_Store_2/Thread_Safe_KV_Store_2.h
	${CC} ${cFlag} Thread_Safe_KV_Store_2/Thread_Safe_KV_Store_2.cpp -o Thread_Safe_KV_Store_2.o

Thread_Safe_Queue.o: Thread_Safe_Queue/Thread_Safe_Queue.cpp Thread_Safe_Queue/Thread_Safe_Queue.h
	${CC} ${cFlag} Thread_Safe_Queue/Thread_Safe_Queue.cpp -o Thread_Safe_Queue.o

Threaded_Server.o: Threaded_Server/Threaded_Server.cpp
	${CC} ${cFlag} Threaded_Server/Threaded_Server.cpp

Threaded_Server_G.o: Threaded_Server/Threaded_Server_Global_Lock.cpp
	${CC} ${cFlag} Threaded_Server/Threaded_Server_Global_Lock.cpp -o Threaded_Server_G.o

Threaded_Server_RW.o: Threaded_Server/Threaded_Server_RW.cpp
	${CC} ${cFlag} Threaded_Server/Threaded_Server_RW.cpp -o Threaded_Server_RW.o

Threaded_Server_Disk.o: Threaded_Server/Threaded_Server_Disk.cpp
	${CC} ${cFlag} Threaded_Server/Threaded_Server_Disk.cpp -o Threaded_Server_Disk.o

httpreq.o: httpreq/httpreq.cc httpreq/httpreq.hpp
	${CC} ${cFlag} httpreq/httpreq.cc -o httpreq.o

httpresp.o: httpreq/httpresp.cc httpreq/httpresp.hpp
	g++ -std=c++11 -g -c  httpreq/httpresp.cc -o httpresp.o

md5.o: md5/md5.cpp md5/md5.h
	${CC} ${cFlag} md5/md5.cpp -o md5.o

Disk_Read_Write.o: Disk_Read_Write/Disk_Read_Write.cpp Disk_Read_Write/Disk_Read_Write.h
	${CC} ${cFlag} Disk_Read_Write/Disk_Read_Write.cpp -o Disk_Read_Write.o -pthread

clean:
	rm *.o server.out server_g.out server_rw.out server_disk.out
