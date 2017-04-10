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
#include "../Thread_Safe_KV_Store_2/Thread_Safe_KV_Store_2.cpp"
#include "../md5/md5.h"
#include "../md5/md5.cpp"

int sockfd, new_sockfd;
Thread_Pool* thread_pool;
Thread_Safe_Queue<int>* thread_times; // NEED TO EDIT //Big Integer?? && just update Q2???
int GET_COUNT=0;
int POST_COUNT=0;
int DELETE_COUNT=0;


/*
 * struct used to store pointers and data for the post, get, and delete functions.
 * This container allows the threads to process the required data and return it to the user
 */
typedef struct{
    Thread_Safe_KV_Store_2<std::string, int>* kv_store;
    Thread_Safe_KV_Store_2<std::string, std::string>* md5_store;
    std::string method;
    std::string key;
    int value;
    int return_value;
    bool is_found;
    int socket;
    std::chrono::time_point<std::chrono::system_clock> start_time;
} task_container;


void error(const char *msg) {
    perror(msg);
    exit(1);
}

void SIGINT_handler(int signo)
{
    if (signo == SIGINT){

        int N = GET_COUNT + POST_COUNT + DELETE_COUNT;
        thread_times->calculate_statistics();
        delete thread_pool;
        printf("\n-----Statistics-----\n+ + Number of Requests + +\nGET: %i\nPOST: %i\nDELETE: %i\nTOTAL: %i\n\n", GET_COUNT, POST_COUNT, DELETE_COUNT, N);
        printf("\n+ + Thread Times + +\nMin: %li\nMax: %ll\nAvg: %Lf\nMed: %Lf\n", thread_times->minimum(), thread_times->maximum(), thread_times->mean(), thread_times->median());
//        close(sockfd);
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
    write (container->socket, response_as_string.c_str(), response_as_string.length() );
    auto end_time = std::chrono::system_clock::now();
    thread_times->enqueue( (end_time - container->start_time).count() );
    printf("%i\n", (end_time - container->start_time).count() );
    printf("Sent Response...\n");
    delete container;
}


/*
 * specialized thread function for the GET method
 * Accepts a pointer to the struct task_container
 */
void* task_GET( void* input){
    printf("Performing task_GET\n");
    task_container* container = (task_container*)input;
    container->start_time = std::chrono::system_clock::now();
    int x = container->kv_store->lookup(container->key, (container->return_value));
    container->is_found = (x==0) ? true:false;
    container->kv_store->accumulate("GET_COUNT", 1);
    respond_to_request(container);
    printf("Completed task_GET\n");
    close(new_sockfd);
    return nullptr;
}



/*
 * specialized function for the POST method
 * Accepts a pointer to the struct task_container
 */
void* task_POST( void* input ){
    printf("Performing task_POST\n");
    task_container* container = (task_container*)input;
    container->start_time = std::chrono::system_clock::now();
    container->kv_store->insert(container->key, container->value);
    container->md5_store->insert(container->key, md5( container->key ));
    container->kv_store->accumulate("POST_COUNT", 1);
    respond_to_request(container);
    printf("Completed task_POST\n");
    close(new_sockfd);
    return nullptr;
}




/*
 * specialized method for the DELETE method
 * Accepts a pointer to the struct task_container
 */
void* task_DELETE( void* input ){
    task_container* container = (task_container*)input;
    int x = container->kv_store->lookup(container->key, (container->return_value));
    container->start_time = std::chrono::system_clock::now();
    container->is_found = (x==0) ? true:false;
    container->kv_store->remove(container->key);
    container->kv_store->accumulate("DELETE_COUNT", 1);
    respond_to_request(container);
    printf("Completed task_DELETE\n");
    close(new_sockfd);
    return nullptr;
}





/*
 * main function needed to initialize the thread pool and run an infinite loop for the the socket
 * the running binary to accept new connections and process the requests
 */

int main(int argc, char *argv[])
{
    int port_number;
    socklen_t client_length;
    struct sockaddr_in server_address, client_address;


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
        error("ERROR! Unable to open the listed socker\nw");


    bzero((char *) &server_address, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port_number);

    if (bind(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        error("ERROR on binding");


//    Initialize the thread pool and launch all threads.
    thread_pool = new Thread_Pool();
    thread_pool->init(number_of_threads);
    thread_times = new Thread_Safe_Queue<int>(); //NEED TO EDIT BigDecimal???

    //Initialize Thread Safe KV stores for the key and string as well as the hash values
    Thread_Safe_KV_Store_2<std::string, int>* key_value_store = new Thread_Safe_KV_Store_2<std::string, int>();
    Thread_Safe_KV_Store_2<std::string, std::string>* md5_hash_store = new Thread_Safe_KV_Store_2<std::string, std::string>();

    //Add the counter vars for statistics of submitted requests
    key_value_store->insert("GET_COUNT" , 0);
    key_value_store->insert("POST_COUNT" , 0);
    key_value_store->insert("DELETE_COUNT" , 0);


    //Listen for incoming connections
    listen(sockfd,5);
    client_length = sizeof(client_address);
    signal(SIGINT, SIGINT_handler);

    //Infinite loop in order for server to receive all requests
    while(true){

//      create new connection
        new_sockfd = accept(sockfd,(struct sockaddr *) &client_address,&client_length);

        //Receive the http request
        HTTPReq* request = new HTTPReq(new_sockfd);
        request->parse();
        std::string request_method = request->getMethod();
        double request_version = request->getVersion();
        std::string request_uri = request->getURI();
        int request_body = atoi(request->getBody().c_str());


        //Package all data into the struct task_container
        task_container* task_data = new task_container();
        task_data->kv_store = key_value_store;
        task_data->md5_store = md5_hash_store;
        task_data->key = request_uri.substr(1);
        task_data->value = request_body;
        task_data->socket = new_sockfd;
        task_data->method = request_method;


        //handle process accordingly to the request
        if(request_method.compare("GET")==0){
            GET_COUNT++;
            thread_pool->add_task(&task_GET, (void*)task_data);
        }
        else if(request_method.compare("POST")==0){
            POST_COUNT++;
            thread_pool->add_task(&task_POST, (void*)task_data);
        }
        else if(request_method.compare("DELETE")==0){
            DELETE_COUNT++;
            thread_pool->add_task(&task_DELETE, (void*)task_data);
        }
    }

//    Unaccessible methods, but needed to ensure that all threads will join the socket will close correctly
//    fo rfuture implementations where there will be an breal/exit clause fo rthe infinite loop

    close(sockfd);
    return 0;
}




