/***************************************************************************
 *
 * Sequential version of Matrix-Matrix multiplication
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define SIZE 1024

static double a[SIZE][SIZE];
static double b[SIZE][SIZE];
static double c[SIZE][SIZE];

void* compute_row(void* arg){
    int row = *(int*)arg;
    free(arg);

    for (int j = 0; j < SIZE; j++) {
        c[row][j] = 0.0;
        for (int k = 0; k < SIZE; k++) {
            c[row][j] += a[row][k] * b[k][j];
        }
    }
    return NULL;
}


void* init_row(void* arg) {
    int row = *(int*)arg;
    free(arg);

     for (int j = 0; j < SIZE; j++) {
        a[row][j] = 1.0;
        b[row][j] = 1.0;
    }
    return NULL;
}
static void
init_matrix(void)
{
    pthread_t threads[SIZE];

    for (int i = 0; i < SIZE; i++) {
        int* row = malloc(sizeof(int));
        *row = i;
        if (pthread_create(&threads[i], NULL, init_row, row) != 0) {
            fprintf(stderr, "Error creating thread for row %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < SIZE; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Error joining thread for row %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
}

static void 
matmul_parallel() {
    pthread_t threads[SIZE];

    // Create threads, one for each row
    for (int i = 0; i < SIZE; i++) {
        int* row = malloc(sizeof(int)); // Allocate memory for the row index
        *row = i;
        if (pthread_create(&threads[i], NULL, compute_row, row) != 0) {
            fprintf(stderr, "Error creating thread for row %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < SIZE; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Error joining thread for row %d\n", i);
            exit(EXIT_FAILURE); 
        }
    }
}

static void
print_matrix(void)
{
    int i, j;

    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++)
	        printf(" %7.2f", c[i][j]);
	    printf("\n");
    }
}

int
main(int argc, char **argv)
{   
    pthread_t threads[SIZE];
    clock_t start, end;
    double parallel_time_used;

    init_matrix();
    start = clock();
    matmul_parallel();
    end = clock();

    parallel_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Time taken: %.6f seconds\n", parallel_time_used);

    return 0;
    //print_matrix();
}
