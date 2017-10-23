/* producer.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "shared.h"

int main()
{
	key_t key = SHM_SHARED_KEY;
	int shmid = shmget( key, sizeof(struct shared_data), IPC_CREAT | IPC_EXCL | 0660 ); 
	//the flags mean: create the memory if it isn't there, but dont make it if it is there. no repeats
	//int shmid = shmget( key, sizeof(struct shared_data), IPC_CREAT | 0660 ); 
	printf( "shmget() returned %d\n", shmid );

	if (shmid == -1)
	{
		perror( "shmid() failed" );
		return EXIT_FAILURE;
	}

	struct shared_data* shm; //making a pointer to our struct
	shm = shmat( shmid, NULL, 0 ); 	//attaching our pointer to the shared memory
									//shm is now an address to our shared memory

	if ( shm == (struct shared_data *)-1 )
	{
		perror( "shmat() failed" );
		return EXIT_FAILURE;
	}

	int item = 1;
	shm->in = shm->out = shm->count = 0;

	while( item < 10000 ) //the producer
	{
		while (shm->count == BUFFER_SIZE)
		{
			/*busy wait*/
		}

		printf("PRODUCER: producing item %d\n", item);
		shm->buffer[shm->in] = item;
		item++;
		shm->in = (shm->in+1)%BUFFER_SIZE; /*circular array*/
		shm->count++;
	}

	/*detach from the shared memory segment*/
	int rc = shmdt(shm);
	if (rc == -1)
	{
		perror("shmdt() failed");
		return EXIT_FAILURE;
	}

	/*remove the shared memory segment*/
	if (shmctl(shmid, IPC_RMID, 0) == -1 )
	{
		perror("shmctl() failed");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}