
Multicore Programming - Lab 4 - A Complete Storage System
@Author James Maciej Krywin
@Net ID: jmk745

---------------------------------------------
Production Information
	-Written on CLion IDE on Mac OS
	-Compiles on NYU CIMS - GCC-6.2.0
	-Written in C++

---------------------------------------------
How to Compile and Run

	-In terminal navigate to directory lab4
        and run the following terminal command: "make server_rw.out" and "make server_disk.out"

	       These two make commands will run the makefile and compile all of the present files into 2 executables.
		1. server_rw.out
		2. server_disk.out


		server_disk.out is the server seen from lab3 with the addition of an on disk store.		

		server_rw.out is a modification of server_disk.out
			Instead of just writing directly to the disk, the server_rw.out will store keys into a key-value store. This addendum acts as a cache and 
			decreases the time that a thread would potentially spend waiting for a disk I/O to complete.

	-Run the compiled executable with the following commands:
                        "./server_rw.out -p <port_number> -n <number_of_threads>"
			
			"./server_disk.out -p <port_number> -n <number_of_threads>"

	

***Note to get the server side statistics,press ctrl + c to send a SIGINT.
The server will stop, print out all results, clean the stored data, and ask if you want to continue. Enter y to exit out of server safely or any other key to continue running further tests.


---------------------------------------------
Description

    This lab4 submission contains 8 directories that compile into a multithreaded server.

    The Thread_Pool directory holds files responsible for the Thread_Pool class. An instance of this class
    handles all spawned threads (conditon variables) and delegates the tasks assigned to its queue (using the add_task method)

    The Thread_Safe_KV_Store_2 directory holds an updated implementation of the older ThreadSafeKVStore submitted in lab1.
    All mutexes have been replaced with read-write locks to improve concurrency.

    The Thread_Safe_Queue directory holds a new, but relatively the same implementation, of the previously submitted ThreadSafeListening Queue. This version allows users to add tasks and removes the listening capability.

    The Threaded_Server holds the main .cpp file that hodls the main function need for executing all of the given DS in a coherent running server.

    The Data directory is empty but holds all of the potential text files used to store the keys and values.

    The Disk_Read_Write directory holds the newly implemented disk I/O functions for this lab. 

    The remaining directories were written by outside sources.


    - Thread_Pool
        - makefile:             used to compile the files of this folder
        - test_for_Thread_Pool: main function that inits and test runs the DS
        - Thread_Pool.cpp:      class that holds all running threads and queues assigned tasks.
        - Thread_Pool.h:        responsible for tasks and thread scheduling

    - Thread_Safe_KV_Store_2
        - makefile:                          used to compile the files of this folder
        - test_for_Thread_Safe_KV_Store_2:   main function that inits and test runs the DS
        - Thread_Safe_KV_Store_2.cpp:        an upgraded version from the previous submitted ThreadSafeKVStore
        - Thread_Safe_KV_Store_2.h:          (DELTA) -> changed mutex locks for read-write locks

    - Thread_Safe_KV_Store
        - makefile:                          used to compile the files of this folder
        - ThreadSafeKVStore.cpp:        	ThreadSafeKVStore from lab1
        - ThreadSafeKVStore_2.h		contains mutex locks for read-write locks


    - Thread_Safe_Queue
        - makefile:                     used to compile the files of this folder
        - test_for_Thread_Safe_Queue:   main function that inits and test runs the DS
        - Thread_Safe_Queue.cpp:        new implementation of a Thread Safe Queue (does not listen)
        - Thread_Safe_Queue.h:          

    - Threaded_Server
        - makefile:                 		used to compile the files of this folder
	- Threaded_Server_Global_Lock.cpp: 	modified server to contain mutex locks for performance comparions
        - Threaded_Server.cpp:      		implementation of a running server
                                        		during running this binary, it inits a thread pool and the Thread_Safe_KV_Store_2 classes
                                        		runs in an infinite loop in order to accept all incoming requests

    - Disk_Read_Write.cpp
	- makefile:				used to compile the files of this folder
	- main.cpp:				main function that inits and tests runs the provided read/write functions
	- Disk_Read_Write.cpp:			contains the read/write/delete functions to access the disk files 
	- Disk_Read_Write.h:

   - Data	
	- this directory holds all the files used to store the keys and values.

    - md5
        - provided by (http://www.bzflag.org)

    - httpreq
        - provided by instructor

    - lab2_tools
        - provided by instructor
