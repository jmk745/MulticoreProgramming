/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <chrono>

#include "../httpreq/httpreq.hpp"
#include "../httpreq/httpresp.hpp"

#include "../Thread_Pool/Thread_Pool.h"
#include "../Thread_Safe_KV_Store_2/Thread_Safe_KV_Store_2.h"
#include "../md5/md5.h"
#include "../md5/md5.cpp"
#include "../Disk_Read_Write/Disk_Read_Write.h"

Thread_Pool* thread_pool;
Thread_Safe_Queue<int>* thread_times;
Thread_Safe_Queue<int>* sockets;
bool running_flag = true;

//global variables that are unsecured without mutex since they are not
int GET_COUNT=0;
int POST_COUNT=0;
int DELETE_COUNT=0;

//Global
//Condition Var and 2 mutecies needed for disk file store
pthread_mutex_t mutex1, mutex2, mutex3;
pthread_cond_t condition;

int sockfd;

/*
 * struct used to store pointers and data for the post, get, and delete functions.
 * This container allows the threads to process the required data and return it to the user
 */
typedef struct{
    Thread_Safe_KV_Store_2<std::string, int>* kv_store;
    Thread_Safe_KV_Store_2<std::string, std::string>* md5_store;
    Thread_Safe_Queue<int>* sockets;
    std::string method;
    std::string key;
    int value;
    int return_value;
    bool is_found;
    int new_sockfd;
    std::chrono::time_point<std::chrono::system_clock> start_time;
} task_container;



void error(const char *msg) {
    perror(msg);
    exit(1);
}


void SIGINT_handler(int signo)
{
    if (signo == SIGINT) {
        int N = GET_COUNT + POST_COUNT + DELETE_COUNT;
        thread_times->calculate_statistics();
        printf("\n-----Statistics-----\n+ + Number of Requests + +\nGET: %i\nPOST: %i\nDELETE: %i\nTOTAL: %i\n\n", GET_COUNT, POST_COUNT, DELETE_COUNT, N);
        printf("\n+ + Thread Times + +\nMin: %li\nMax: %li\nAvg: %Lf\nMed: %Lf\n", thread_times->minimum(), thread_times->maximum(), thread_times->mean(), thread_times->median());

        //Clean out and Restart ad counts
        GET_COUNT = 0;
        POST_COUNT = 0;
        DELETE_COUNT = 0;
        while (!thread_times->isEmpty()) {
            thread_times->dequeue();
        }

        printf("Data Cleared\n\n");

        printf("Press 'y' to close socket or any other key to continue running..\n");
        char c;
        std::cin >> c;
        if (c == 'y' || c == 'Y') {
            close(sockfd);
            running_flag = false;
        }
    }
}


/*
 * Send HTTP Response
 * After processing the http request, generate a response for the client and send it out through the socket.
 * Accepts a pointer to the struct task_container
 */
void respond_to_request(task_container* container){

    HTTPResp* response;
    std::string final_return_message;
    if ( container->method.compare("POST")==0 ){
        final_return_message = "\n";
    }
    else if ( !container->is_found ){
        response = new HTTPResp(404, "Not Found\n", true);
    }
    else if ( container->method.compare("GET")==0 ){
        std::string value_string ( std::to_string( container->return_value) );
        std::string ending ("\n");
        final_return_message = value_string + ending;
    }
    else if ( container->method.compare("DELETE")==0 ){
        final_return_message = "Deleted\n";
    }

    response = new HTTPResp(200, final_return_message, true);
    std::string response_as_string = response->getResponse();
    write (container->new_sockfd, response_as_string.c_str(), response_as_string.length() );
    auto end_time = std::chrono::system_clock::now();
    thread_times->enqueue( (end_time - container->start_time).count() );
    printf("%i\n", (end_time - container->start_time).count() );
    printf("Sent Response...\n");
}



/*
 * thread created for a new session to take care of its requests
 */
void* thread (void* input) {

    //retrieve the task container
    task_container* container = (task_container*) input;
    int new_sockfd;

    while (running_flag) {

        container->sockets->listen(new_sockfd);

        //Receive the http request
        HTTPReq* request = new HTTPReq(new_sockfd);
        //check if EIO
        if (request->parse() < 0) {
            printf("Parse Failed\n");
            close(new_sockfd);
        }
        else {
            //returns a value and handle if negative one
            std::string request_method = request->getMethod();
            double request_version = request->getVersion();
            std::string request_uri = request->getURI();
            int request_body = atoi(request->getBody().c_str());

            //Package all data into the struct task_container
            container->new_sockfd = new_sockfd;
            container->key = request_uri.substr(1);
            container->value = request_body;
            container->new_sockfd = new_sockfd;
            container->method = request_method;
            container->start_time = std::chrono::system_clock::now();

            char data_key[50];
            strcpy(data_key, "data/");
            strcat(data_key, container->key.c_str());

            char hash_key[50];
            strcpy(data_key, "data/");
            strcat(hash_key, container->key.c_str());
            strcat(hash_key, ".hash");

            //handle process accordingly to the request
            if(request_method.compare("GET")==0){ //get
                GET_COUNT++;
                printf("Performing task_GET\n");
                int x = read_from_file_and_cache (container->key.c_str(), &(container->return_value), container->kv_store, &mutex1, &condition, &mutex2);
                container->is_found = (x==0) ? true:false;
                respond_to_request(container);
                printf("Completed task_GET\n");
            }
            else if(request_method.compare("POST")==0){ //post
                POST_COUNT++;
                printf("Performing task_POST\n");

                write_to_file_and_cache(container->key.c_str(), container->value, container->kv_store, &mutex1, &condition, &mutex2);

                //worry about the hashes later
//                container->md5_store->insert(container->key, md5( container->key ));
                respond_to_request(container);
                printf("Completed task_POST\n");
            }
            else if(request_method.compare("DELETE")==0){ //Delete
                DELETE_COUNT++;

                int x = delete_from_file_and_cache(container->key.c_str(), container->kv_store, &mutex1, &condition, &mutex2);
                container->is_found = (x==0) ? true:false;
                respond_to_request(container);
                printf("Completed task_DELETE\n");
            }
            container->sockets->enqueue(new_sockfd);
        }
        printf("Got Here!\n");
    }
    close(new_sockfd);
    pthread_exit(0);
}



/*
 * main function needed to initialize the thread pool and run an infinite loop for the the socket
 * the running binary to accept new connections and process the requests
 */

int main(int argc, char *argv[])
{
    int new_sockfd;
    int port_number;
    socklen_t client_length;
    struct sockaddr_in server_address, client_address;


    //init of global mutecies and condition variable
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    pthread_mutex_init(&mutex3, NULL);
    pthread_cond_init(&condition, NULL);


    //implemented getOpt for -n <number of threads for the sever>
    // as well as -p for the port number
    int option;
    bool p_flag = false;
    bool n_flag = false;
    bool garbage_flag = false;
    int number_of_threads;
    while( (option = getopt(argc, argv, "p:n:")) != -1){

        if(option == 'p'){
            port_number = atoi(optarg);
            p_flag=true;
        }
        if(option == 'n'){
            number_of_threads = atoi(optarg);
            n_flag=true;
        }
    }

    //Check to see if needed commnadline arguments have been passed in
    if( (!p_flag) || (!n_flag) )
        error("Invalid number of arguments\nPlease enter the following: ./server.out [-p <port_number>] [-n <number_of_threads>]");



    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR! Unable to open the listed socket\nw");


    bzero((char *) &server_address, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port_number);

    if (bind(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        error("ERROR on binding");


//    Initialize the thread pool and launch all threads.
    thread_pool = new Thread_Pool();
    thread_pool->init(number_of_threads);
    thread_times = new Thread_Safe_Queue<int>();
    sockets = new Thread_Safe_Queue<int>();

    //Initialize Thread Safe KV stores for the key and string as well as the hash values
    Thread_Safe_KV_Store_2<std::string, int>* key_value_store = new Thread_Safe_KV_Store_2<std::string, int>();
    Thread_Safe_KV_Store_2<std::string, std::string>* md5_hash_store = new Thread_Safe_KV_Store_2<std::string, std::string>();


    client_length = sizeof(client_address);
    signal(SIGINT, SIGINT_handler); //connect handler to signal


    //load all threads in the
    for (int i=0; i<number_of_threads; i++) {

        task_container* task_data= new task_container();
        task_data->kv_store = key_value_store;
        task_data->md5_store = md5_hash_store;
        task_data->sockets = sockets;
        //create a new thread to handle the connection and make use of the spawned thread_pool
        thread_pool->add_task(thread, (void*) task_data);
    }

    //Infinite loop in order for server to receive all requests
    while(running_flag){

        //Listen for incoming connections
        listen(sockfd,128);

//      create new connection
        new_sockfd = accept(sockfd,(struct sockaddr *) &client_address,&client_length);
        sockets->enqueue(new_sockfd);
    }


    //close main server socket
    close(sockfd);
    return 0;
}




