/*octuplets-thread.c*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CHILDREN 8

/* function executed by each thread*/
void* whattodo(void* arg);

int main()
{
	pthread_t tid[CHILDREN]; /*keep track of the thread IDs.*/
	int i, rc;

	int *t; 

	for (i = 0; i < CHILDREN; i++) {
		t = malloc(sizeof(int));
		*t = 2+(i*2); /*2,4,6,8,..*/

		rc = pthread_create( &tid[i], NULL, whattodo, t);

		if (rc != 0) {
			fprintf( stderr, "MAIN: Could not create thread (%d)\n", rc);
			return EXIT_FAILURE;
		}
	}

	/*wait for child threads to terminate*/
	for (i = 0; i < CHILDREN; i++) {
		//pthread_join( tid[i], NULL ); 
										/*blocking call... if NULL in second spot, then we dont
										care about return value*/
		unsigned int* x;
		rc = pthread_join(tid[i], (void**)&x);

		if (rc != 0) {
			fprintf( stderr, "MAIN: Could not join thread (%d)\n", rc);
			return EXIT_FAILURE;
		}

		printf("MAIN: Joined child thread %u, which returned %u\n", (unsigned int)tid[i], *x);
		free(x);
	}

	printf("MAIN: All threads terminated successfully.\n");
	fflush(NULL);

	return EXIT_SUCCESS;
}

/*function executed by each thread*/
void* whattodo(void* arg)
{
	int t = *(int*)arg;
	free(arg);

	printf("THREAD %d: I'm going to nap for %d seconds.\n", (int)pthread_self(), t);
	sleep(t);

	printf("THREAD %d: I'm awake.\n", (int)pthread_self());

	/*dynamically allocate space to hold a return value*/
	unsigned int* x = malloc(sizeof(unsigned int));
	*x = pthread_self(); /*return the thread ID of the child*/

	pthread_exit(x);
	return(x);
	//return NULL; //same as pthread_exit(NULL);
}