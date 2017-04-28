#include <stdio.h>
#include <pthread.h>
#include "Disk_Read_Write.cpp"




int main() {

	pthread_cond_t condition;
	pthread_mutex_t mutex1, mutex2;

	pthread_cond_init(&condition, NULL);
	pthread_mutex_init(&mutex1, NULL);
	pthread_mutex_init(&mutex2, NULL);


	int val;

	write_to_file("../data/JAMES", 987654321, &mutex1, &condition, &mutex2);
	printf("Wrote to file...\n");
	read_from_file("../data/JAMES", &val, &mutex1, &condition, &mutex2);
	printf("Read value from file.. --%i--\n", val);
	delete_from_file("data/JAMES", &mutex1, &condition, &mutex2);
	printf("Deleted File\n");

	return 0;
}
