Multicore Programming - Lab 1 - ThreadSafe Key Value Store, Thread Safe FIFO Queue, and Multi Threads
@Author James Maciej Krywin
@Net ID: jmk745

---------------------------------------------
Production Information
	-Written on CLION on Mac OS
	-Written in C++
    -Contains:  main.cpp                    - runs the test threads and uses the following implementations
                testThread.cpp              - contains the thread function
                ThreadSafeKVStore.cpp       - implementation of the thread safe hashmap DS
                ThreadSafeKVStore.h
                ThreadSafeListenerQueue.cpp - implementation of a thread safe FIFO queue
                ThreadSafeListenerQueue.h
                makefile                    - quick make

    -Compiles into an executable named: "lab1"

---------------------------------------------
How to Compile and Run

	-In terminal navigate to directory lab1
		and run the following command: "make"
		**This will compile all the provided .cpp and .h files into an executable: "lab1"

	-Run compiled executable with the following command: "./lab1 -n < N >"
        **N represents the number of threads. It accepts input from 1 to the number set for constant MAX_NUMBER_OF_THREADS.
        **This constant is required in order to limit the number of threads called by the user because the thread's running
        times are stored in a static array of a limited size.

	-Program will exit and return an error if a number lower than 1 or greater than 20 is used.
    -Program will also return an error if the -n arguments is excluded. So make sure it is used.
    **It will also complain if too many arguments were used during execution.

---------------------------------------------
Description

	-This lab contains implementations of a Thread Safe FIFO Queue and a Thread Safe KV Storage DS.

    -This lab takes a number of threads and spawns the instructed number of threads.
        Each thread will generate a random user key and value or look up previous entries for 10,000 iterations.
        These values will be stored in the Thread Safe KV Store concurrently with other running threads.
        After completing this task, the thread will enqueue its recorded running sum on the thread safe FIFO Queue.

    -During the termination of each thread, it will print out its process time and completion time as well as if any
        keys were found missing during look up.

	-At the completion of all thread, the program will sum up the enqueue'd values and print the sum.
        The program will then traverse the entire key set of the TS-KV Store and sum up the stored values. This will then
        be compared to the printed value and check whether the values indeed do match up.
        Correct value indicate a successful implementation of a thread safe DS.

----------------------------------------------
template <typename K, typename V> class ThreadSafeKVStore{

    public:
        ThreadSafeKVStore< K, V>();
            - This method is a constructor that creates an instance of the defined template class

        ~ThreadSafeKVStore<K, V>();
            - This method is the destructor the frees the previously allocated memory

        int insert(const K key, const V value);
            - This method returns an int upon termination (0 upon success and -1 upon error).
                It will check if the DS contains the current key. It it doesn't, then it will insert it into the DS.
                Else it will do nothing.
                This method is Thread Safe.

        int accumulate(const K key, const V value);
            - Similarly to insert, this method will insert elements into the DS and return a int value of
            0 or 1 upon termination (success and error respectively). However, if the element already exists in the DS, then the method
            will add the new value to the previous one and store the sum.
            This method is Thread Safe.


        int lookup(const K key, V& value);
            - This method will look up the value in the DS and return whether the value is found (0 upon success and -1 if not found)
            This method is Thread Safe.

        int remove(const K key);
            - This method will remove the passed in key from the DS and return an int value (0 upon success and -1 upon error)
            This method is Thread Safe.

        int size();
            - This method returns the current size of the DS
            This method is Thread Safe.

        typename std::unordered_map<K, V>::iterator begin();
            - This method returns an iterator pointing to the beginning of the key set
            This method is NOT Thread Safe.

        typename std::unordered_map<K, V>::iterator end();
            - This method returns an iterator pointing to the end of the key set.

    private:
        std::unordered_map<K, V>* map;
            - This unordered map will hold all insert keys and values
            - Accessing this alone is NOT Thread Safe.

        bool contains(const K key);
            - An NOT thread safe method that checks whether an element is contained within the DS

        pthread_mutex_t mutexLock;
            - This property of the Thread Safe DS enables the unordered_map to be accessed in a thread safe manner. All operation locks
            rely on this mutex instance.



----------------------------------------------
template <typename T> class ThreadSafeListenerQueue {

    public:
        ThreadSafeListenerQueue();
        - This method is a constructor that creates an instance of the defined template class

        ~ThreadSafeListenerQueue();
        - This method is the destructor the frees the previously allocated memory

        int push(const T element);
        - This method return a int value upon completion (0 on success and -1 if experience an error).
        - This method is Thread Safe.

        int pop(T& element);
        - This method returns a 0 or -1 upon completion if it experienced a successful operation or error.
        This method is Thread Safe.

        int listen(T& element);
        This method returns an int upon completion (0 for success and -1 for an error).
        - This method functions similarly

    private:
        std::list<T>* queue;

        pthread_mutex_t mutexLock;

};
----------------------------------------------
Things to Note
