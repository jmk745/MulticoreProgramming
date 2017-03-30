
Multicore Programming - Lab 2 - Thread_Pool
@Author James Maciej Krywin
@Net ID: jmk745

---------------------------------------------
Production Information
	-Written on CLion IDE on Mac OS
	-Unable to compiler on CIMS due to old compiler
	-Written in C++

---------------------------------------------
How to Compile and Run

	-In terminal navigate to directory lab2
        and run the following terminal command: "make"
        This will run the makefile and compile all of the present files into an executable named "server.out"


	-Run the compiled executable with the following command:
                        "./server.out -p <port_number> -n <number_of_threads>"

    -Check server response with curl

---------------------------------------------
Description

    This lab2 submission contains 7 directories that compile into a multithreaded server.

    The Thread_Pool directory holds files responsible for the Thread_Pool class. An instance of this class
    handles all spawned threads (conditon variables) and delegates the tasks assigned to its queue (using the add_task method)

    The Thread_Safe_KV_Store_2 directory holds an updated implementation of the older ThreadSafeKVStore submitted in lab1.
    All mutexes have been replaced with read-write locks to improve concurrency.

    The Thread_Safe_Queue directory holds a new, but relatively the same implementation, of the previously submitted ThreadSafeListening Queue. This version allows users to add tasks and removes the listening capability.

    The Threaded_Server holds the main .cpp file that hodls the main function need for executing all of the given DS in a coherent running server.

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

    - Thread_Safe_Queue
        - makefile:                     used to compile the files of this folder
        - test_for_Thread_Safe_Queue:   main function that inits and test runs the DS
        - Thread_Safe_Queue.cpp:        new implementation of a Thread Safe Queue (does not listen)
        - Thread_Safe_Queue.h:          

    - Threaded_Server
        - makefile:                 used to compile the files of this folder
        - Threaded_Server.cpp:      implementation of a running server
                                        during running this binary, it inits a thread pool and the Thread_Safe_KV_Store_2 classes
                                        runs in an infinite loop in order to accept all incoming requests

    - md5
        - provided by (http://www.bzflag.org)

    - httpreq
        - provided by instructor

    - lab2_tools
        - provided by instructor
