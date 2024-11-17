#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <time.h>
#define SHMSIZE 128
#define SHM_R 0400
#define SHM_W 0200
#define BUFFER_SIZE 10

int main(int argc, char **argv)
{
	struct shm_struct {
		int buffer[BUFFER_SIZE];
		unsigned empty_slots; //count of empty slots
		unsigned full_slots; //count of full slots
		unsigned head; //Head index for the cirular buffer
		unsigned tail; //Tail index for the circular buffer
	};

	volatile struct shm_struct *shmp = NULL;
	char *addr = NULL;
	pid_t pid = -1;
	int var1 = 0, var2 = 0, shmid = -1;
	struct shmid_ds *shm_buf;

	/* allocate a chunk of shared memory */
	shmid = shmget(IPC_PRIVATE, SHMSIZE, IPC_CREAT | SHM_R | SHM_W);
	shmp = (struct shm_struct *) shmat(shmid, addr, 0);

	/* Init the shared memory structure */
	shmp->empty_slots = BUFFER_SIZE;
	shmp->full_slots = 0;
	shmp->head = 0;
	shmp->tail = 0;

	/* Seed the random number generator */
    srand(time(NULL));

	pid = fork();

	if (pid != 0) {
		/* here's the parent, acting as producer */
		while (var1 < 100) {
			/* write to shmem */
			var1++;
			
			/* Wait until there's an empty slot */
            while (shmp->empty_slots == 0);
			printf("Sending %d\n", var1); fflush(stdout);
			shmp->buffer[shmp->head] = var1;
			shmp->head = (shmp->head + 1) % BUFFER_SIZE;

			/* Update the counters */
			shmp->empty_slots--;
			shmp->full_slots++;

			/* Sleep for a random interval (0.1s to 0.5s) */
            usleep((rand() % 400 + 100) * 1000);
		}

		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	} else {
		/* here's the child, acting as consumer */
		while (var2 < 100) {
			/* read from shmem */
			while (shmp->full_slots == 0); /* busy wait until there is something */
			var2 = shmp->buffer[shmp->tail];
			printf("Received %d\n", var2); fflush(stdout);
			shmp->tail = (shmp->tail + 1) % BUFFER_SIZE;

			/* Update the counters */
			shmp->empty_slots++;
			shmp->full_slots--;

			/* Sleep for a random interval (0.2s to 2s) */
            usleep((rand() % 1800 + 200) * 1000);
		}
		shmdt(addr);
		shmctl(shmid, IPC_RMID, shm_buf);
	}
	return 0;
}
