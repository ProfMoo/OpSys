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

	int t; /*this is statically allocated memory
			all threads have access to this*/

	for (i = 0; i < CHILDREN; i++) {
		t = 2+(i*2); /*2,4,6,8,..*/

		rc = pthread_create( &tid[i], NULL, whattodo, &t);

		if (rc != 0) {
			fprintf( stderr, "MAIN: Could not create thread (%d)\n", rc);
			return EXIT_FAILURE;
		}
	}

	sleep(20);

	return EXIT_SUCCESS;
}

/*function executed by each thread*/
void* whattodo(void* arg)
{
	int t = *(int*)arg;

	printf("THREAD %d: I'm going to nap for %d seconds.\n", (int)pthread_self(), t);
	sleep(t);

	printf("THREAD %d: I'm awake.\n", (int)pthread_self());

	return NULL;
}