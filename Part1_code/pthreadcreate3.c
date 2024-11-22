#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct threadArgs {
	unsigned int id;
	unsigned int numThreads;
	unsigned int squaredID; // Square of the thread ID = Child to Parent
};

void* child(void* params) {
	struct threadArgs *args = (struct threadArgs*) params;
	unsigned int childID = args->id;

	args->squaredID = childID * childID;
	
	printf("Greetings from child #%u of %u, SquaredID = %u\n", childID, args->numThreads, args->squaredID);

	return NULL;
}

int main(int argc, char** argv) {
	pthread_t* children; // dynamic array of child threads
	struct threadArgs* args; // argument buffer
	unsigned int numThreads = 0;

	if (argc > 1) {
    numThreads = atoi(argv[1]);
	} else {
    fprintf(stderr, "Usage: %s <numThreads>\n", argv[0]);
    return EXIT_FAILURE;
	}



	// Allocate mermory for thread handles and argument structs
	children = malloc(numThreads * sizeof(pthread_t)); // allocate array of handles
	args = malloc(numThreads * sizeof(struct threadArgs)); // args vector

	// Create Child threads
	for (unsigned int id = 0; id < numThreads; id++) {
		// create threads
		args[id].id = id;
		args[id].numThreads = numThreads;

		// Create tread and check for errors
		if (pthread_create(&(children[id]), NULL, child, (void*)&args[id]) != 0){
			fprintf(stderr, "Error creating thread %u\n", id);
			free(args);
			free(children);
			return EXIT_FAILURE;	
		}
	}

	printf("I am the parent (main) thread.\n");

	// Wait for child threads
	for (unsigned int id = 0; id < numThreads; id++) {
		if (pthread_join(children[id], NULL) != 0) {
            fprintf(stderr, "Error joining thread %u\n", id);
            free(args);
            free(children);
            return EXIT_FAILURE;
        }
	}

	 // Print the squared IDs collected from child threads
    printf("Parent: Squared IDs from child threads:\n");
    for (unsigned int id = 0; id < numThreads; id++) {
        printf("Thread #%u: Squared ID = %u\n", id, args[id].squaredID);
    }

	// Free allocated memory
	free(args); // deallocate args vector
	free(children); // deallocate array
	return 0;
}
