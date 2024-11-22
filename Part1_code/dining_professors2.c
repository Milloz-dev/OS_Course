#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_PROFESSORS 5

// Mutex chopsticks
pthread_mutex_t chopsticks[NUM_PROFESSORS];

void random_sleep(int minSec, int maxSec){
    int durations = (rand() % (maxSec - minSec +1)) + minSec;
    sleep(durations);
}

void* professor(void* arg){
    int id = *(int*)arg;
    free(arg);

    int leftchopstick = id;
    int rightchopstick = (id +1) % NUM_PROFESSORS;

    // hierarchical order
    if (leftchopstick > rightchopstick) {
        int temp = leftchopstick;
        leftchopstick = rightchopstick;
        rightchopstick = temp;

    }

    while(1) {
        printf("Professor %d: thingking\n", id);
        random_sleep(1,5);

        // Try left chopstick
        printf("Professor %d: trying to pick up left chopstick (%d)\n", id, leftchopstick);
        pthread_mutex_lock(&chopsticks[leftchopstick]);
        printf("Professor %d: picked up left chopstick (%d)\n", id, leftchopstick);

        random_sleep(2, 8);

        // Try right chopstick
        printf("Professor %d: trying to pick up right chopstick (%d)\n", id, rightchopstick);
        pthread_mutex_lock(&chopsticks[rightchopstick]);
        printf("Professor %d: picked up right chopstick (%d)\n", id, rightchopstick);

        // Eat
        printf("Professor %d; eating\n", id);
        random_sleep(5,10);

        // put down chopsticks
        pthread_mutex_unlock(&chopsticks[rightchopstick]);
        printf("Professor %d: put down right chopstick (%d)\n", id, rightchopstick);
        pthread_mutex_unlock(&chopsticks[leftchopstick]);
        printf("Professor %d: put down left chopstick (%d)\n", id, leftchopstick);
    }
    return NULL;
}

int main(){
    pthread_t professors[NUM_PROFESSORS];

    // Init mutex
    for (int i = 0; i < NUM_PROFESSORS; i++){
        pthread_mutex_init(&chopsticks[i],NULL);
    }

    // Professor threads
    for (int i = 0; i < NUM_PROFESSORS; i++){
        int* id = malloc(sizeof(int)); // Allocate memory for professor id
        *id = i;
        pthread_create(&professors[i], NULL, professor, id);
    }

    // Wait for professor threads to complete (won't happen)
    for (int i = 0; i < NUM_PROFESSORS; i++) {
        pthread_join(professors[i], NULL);
    }

    // Cleanup mutexes
    for (int i = 0; i < NUM_PROFESSORS; i++) {
        pthread_mutex_destroy(&chopsticks[i]);
    }

    return 0;
}