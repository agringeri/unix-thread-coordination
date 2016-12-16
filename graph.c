// Anthony Gringeri
// acgringeri

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>

#define MAX_THREADS 26
#define MAX_NUM_DEPENDENCIES 25

// Buffer for reading file -- increase if needed for very large configuration files
#define BUFFER_SIZE 1024

typedef struct {
	int id; // numerical identifier of thread (subtracted ASCII value -- A=0, B=1, etc)
	int value; // value to add to global variable
	int sleep_time; // seconds to compute value
	int num_dependencies; // amount of nodes this node depends on
	int dependent_nodes[MAX_NUM_DEPENDENCIES]; // node_ids that this node depends on
} NODE; 

int global_value = 0; // global value that will be used in calculations

time_t start_time; // starting time in seconds after 01/01/1970

NODE nodes[MAX_THREADS]; // array of all nodes (threads) that are created

sem_t semid[MAX_THREADS]; // initialize semaphores

pthread_mutex_t mutex; // mutex to be used for global_value

// Function that threads will execute
void *compute(int value) {
	// Enter critical region
	pthread_mutex_lock(&mutex);
	// Add value to global varible
	global_value = value + global_value;
	// Exit critical region
	pthread_mutex_unlock(&mutex);
}

void *handler(void *arg) {
	int i, node_num;
	node_num = (long)arg;

	for (i = 0; i < nodes[node_num].num_dependencies; i++) {
		sem_wait(&semid[nodes[node_num].dependent_nodes[i]]);
		sem_post(&semid[nodes[node_num].dependent_nodes[i]]);
	}
	sleep(nodes[node_num].sleep_time); // sleep for allotted time
	compute(nodes[node_num].value); // "compute" value
	sem_post(&semid[node_num]);

	// Calculate completion time of node and print its results
	long int node_time = ((long int)(time(NULL)) - (long int)(start_time)); 
	printf("Node %c computed a value of %d after %li seconds.\n", (nodes[node_num].id + 'A'), nodes[node_num].value, node_time);
}


int main(int argc, char *argv[]) {
	// Calculate starting time 
	start_time = time(NULL);

	// Initialize mutex for global variable
	if (pthread_mutex_init(&mutex, NULL) < 0) {
		perror("pthread_mutex_init");
		exit(1);
	}

	// Check that arguments were provided on command line
	if (argc < 2) {
		printf("Please provide a configuration file.\n");
		exit(1);
	} else if (argc > 2) {
		printf("Using first argument as configuration file. Ignoring all other input...\n");
	}

	FILE* file = fopen(argv[1], "r"); // pointer to config file

	// Check if file exits and can be opened
	if (file == NULL) {
		printf("File not found or could not be opened. Exiting.\n");
		exit(1);
	}
	
	char buffer[BUFFER_SIZE]; // buffer for reading file
	char* c; // holds tokens for analyzing and computation
	int line_count = 0; // Keep track of what line is being read to write to an array

	// Analyze config file and determine amount of threads and sems to create
	while (fgets(buffer, BUFFER_SIZE, file)) {
		int num_dependencies = 0; // number of nodes this node depends on
		NODE new_node; // Node to create from line of config file
		int place = 2; // used to calculate where in the line we are (start at second value)

		// Get first value
		c = strtok(buffer, " ");
		// Check if the first value is an uppercase letter
		if ((c[0] >= 65) && (c[0] <= 90)) {
			// First letter of line
			new_node.id = (c[0] - 65);
		} else {
			printf("Configuration file is not correct format. Exiting.\n");
			exit(1);
		}
		// Check rest of line
		while ((c = strtok(NULL, " ")) != NULL) {

			if (place == 2) {
				if ((atoi(c)) > 0) {
					// First number of line
					new_node.value = atoi(c);
				} else {
					printf("Configuration file is not correct format. Exiting.\n");
					exit(1);
				}
			}

			if (place == 3) {
				if ((atoi(c)) > 0) {
					// Second number of line
					new_node.sleep_time = atoi(c);
				} else {
					printf("Configuration file is not correct format. Exiting.\n");
					exit(1);
				}
			}

			if (place > 3) {
				if ((c[0] >= 65) && (c[0] <= 90)) {
					// Last letters of line
					num_dependencies++;
					new_node.dependent_nodes[(place - 4)] = (c[0] - 65);
				} else {
					printf("Configuration file is not correct format. Exiting.\n");
					exit(1);
				}
			
			} place++; // Next place 

		} 
		new_node.num_dependencies = num_dependencies;

		// Place node into global array of nodes for later use (thread creation)
		nodes[(line_count)] = new_node;
		line_count++;
	}

	// NODE ARRAY FILLED, CLOSE FILE
	fclose(file);

	// Create array of threads
	pthread_t threads[line_count];

	// Initialize semaphores
	int i = 0;
	for(; i < line_count; i++) {
		if (sem_init(&semid[i], 0, 0) < 0) {
			perror("sem_init");
			exit(1);
		}
	}

	// Initialize threads
	long j;
	for (j = 0; j < line_count; j++) {
		if (pthread_create(&threads[(int)j], NULL, handler, (void *)j) != 0) {
			perror("pthread_create");
			exit(1);
		}
	}

	for(i = 0; i < line_count; i++) {
		(void)pthread_join(threads[i], NULL);
		(void)sem_destroy(&semid[i]);
	}

	// Destroy mutex for global variable
	(void)pthread_mutex_destroy(&mutex);
	
	// Calculate total time and print final results
	long int total_time = ((long int)(time(NULL)) - (long int)(start_time));
	printf("Total computation resulted in a value of %d after %li seconds.\n", global_value, total_time);
	return(0);
}
