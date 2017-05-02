#include <stdio.h>
#include <pthread.h>
#include "Disk_Read_Write.h"




int main() {

	pthread_cond_t condition;
	pthread_mutex_t mutex1, mutex2;

	pthread_cond_init(&condition, NULL);
	pthread_mutex_init(&mutex1, NULL);
	pthread_mutex_init(&mutex2, NULL);


	int val;
	const char* test_hash = "test_Hash_test_Hash";
	char return_hash[40];
//    std::string return_hash_string;
	strcpy(return_hash, "NULL");

	const char* key = "../data/JAMES";

    Thread_Safe_KV_Store_2<std::string, int>* k = new Thread_Safe_KV_Store_2<std::string, int>();
    Thread_Safe_KV_Store_2<std::string, std::string>* h = new Thread_Safe_KV_Store_2<std::string, std::string>();



//	write_to_file(key, 987654321, test_hash, &mutex1, &condition, &mutex2);
//	printf("Wrote to file...\n");
//	read_from_file(key, &val, return_hash, &mutex1, &condition, &mutex2);
//	printf("Read value from file.. --%i--  --%s--\n", val, return_hash);
//	delete_from_file(key, &mutex1, &condition, &mutex2);
//	printf("Deleted File\n");

//    write_to_file_and_cache(key, 1020304050, test_hash, k, h, &mutex1, &condition, &mutex2);

    printf("STARTING\n");
    smart_write_to_file_and_cache(key, 123, test_hash, k, h, &mutex1, &condition, &mutex2);
    printf("Wrote to file...\n");
    read_from_file_and_cache(key, &val, return_hash, k, h, &mutex1, &condition, &mutex2);
	printf("Read value from file.. --%i--  --%s--\n", val, return_hash);
    delete_from_file_and_cache(key, k, h, &mutex1, &condition, &mutex2);
//    delete_from_file(key, &mutex1, &condition, &mutex2);
	printf("Deleted File\n");

	printf("Test Completed\n");
	return 0;
}
