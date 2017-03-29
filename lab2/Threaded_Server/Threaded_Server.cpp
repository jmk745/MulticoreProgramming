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

#include "../httpreq/httpreq.hpp"
#include "../httpreq/httpresp.hpp"

#include "../Thread_Pool/Thread_Pool.h"
#include "../Thread_Safe_KV_Store_2/Thread_Safe_KV_Store_2.h"
#include "../Thread_Safe_KV_Store_2/Thread_Safe_KV_Store_2.cpp"
#include "../md5/md5.h"
#include "../md5/md5.cpp"




typedef struct{
    Thread_Safe_KV_Store_2<std::string, int>* kv_store;
    Thread_Safe_KV_Store_2<std::string, std::string>* md5_store;
    std::string method;
    std::string key;
    int value;
    int return_value;
    bool is_found;
    int socket;
} task_container;


void error(const char *msg) {
    perror(msg);
    exit(1);
}


//Send http response
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
    delete container;
}

void* task_GET( void* input){
    printf("Performing task_GET\n");
    task_container* container = (task_container*)input;
    int x = container->kv_store->lookup(container->key, (container->return_value));
    container->is_found = (x==0) ? true:false;
    respond_to_request(container);
    return nullptr;
}


void* task_POST( void* input ){
    printf("Performing task_POST\n");
    task_container* container = (task_container*)input;
    container->kv_store->insert(container->key, container->value);
    container->md5_store->insert(container->key, md5( container->key ));
    respond_to_request(container);
    printf("Completed task_POST\n");
    return nullptr;
}


void* task_DELETE( void* input ){
    task_container* container = (task_container*)input;
    int x = container->kv_store->lookup(container->key, (container->return_value));
    container->is_found = (x==0) ? true:false;
    container->kv_store->remove(container->key);
    respond_to_request(container);
    return nullptr;
}






int main(int argc, char *argv[])
{
    int sockfd, new_sockfd, port_number;
    socklen_t client_length;
    char buffer[256];
    struct sockaddr_in server_address, client_address;

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

    if( (!p_flag) || (!n_flag) )
        error("Invalid number of arguments\nPlease enter the following: ./Threaded_Server [-p <port_number>] [-n <number_of_threads>]");


    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");


    bzero((char *) &server_address, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port_number);

    if (bind(sockfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        error("ERROR on binding");

    Thread_Pool* thread_pool = new Thread_Pool();
    thread_pool->init(number_of_threads);
    Thread_Safe_KV_Store_2<std::string, int>* key_value_store = new Thread_Safe_KV_Store_2<std::string, int>();
    Thread_Safe_KV_Store_2<std::string, std::string>* md5_hash_store = new Thread_Safe_KV_Store_2<std::string, std::string>();
    key_value_store->insert("hello", 123456789);

    listen(sockfd,5);
    client_length = sizeof(client_address);


    //Infinite loop in order for server to receive all requests
    while(true){
        new_sockfd = accept(sockfd,(struct sockaddr *) &client_address,&client_length);

        //Receive the http request
        HTTPReq* request = new HTTPReq(new_sockfd);
        request->parse();
        std::string request_method = request->getMethod();
        double request_version = request->getVersion();
        std::string request_uri = request->getURI();
        int request_body = atoi(request->getBody().c_str());

        task_container* task_data = new task_container();
        task_data->kv_store = key_value_store;
        task_data->md5_store = md5_hash_store;
        task_data->key = request_uri.substr(1);
        task_data->value = request_body;
        task_data->socket = new_sockfd;a
        task_data->method = request_method;

        if(request_method.compare("GET")==0){
            thread_pool->add_task(&task_GET, (void*)task_data);
        }
        else if(request_method.compare("POST")==0){
            thread_pool->add_task(&task_POST, (void*)task_data);
        }
        else if(request_method.compare("DELETE")==0){
            thread_pool->add_task(&task_DELETE, (void*)task_data);
        }
    }

    delete thread_pool;

    close(new_sockfd);
    close(sockfd);
    return 0;
}



