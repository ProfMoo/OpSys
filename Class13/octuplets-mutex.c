/*octuplets-mutex.c*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define CHILDREN 8

/* function executed by each thread*/
void* whattodo(void* arg);

/*global mutex variable to synchronize child threads*/
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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
		rc = pthread_join(tid[i], (void**)&x); /*gets rid of thread in table. before, it was just a zombie.
												also allows a mechanism to return something from child thread
												very similar to a waitpid in regards to the waiting for children*/

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

/*critical section code*/
/* e.g., only one thread can sleep() at any given */
void critical_section( int time )
{
	printf( "THREAD %u: Entered critical section.\n", (unsigned int)pthread_self() );
	sleep(time);
	printf( "THREAD %u: Exiting critical section.\n", (unsigned int)pthread_self() );
}

/*function executed by each thread*/
void* whattodo(void* arg)
{
	int t = *(int*)arg;
	free(arg);

	printf("THREAD %d: I'm going to nap for %d seconds.\n", (int)pthread_self(), t);

	pthread_mutex_lock( &mutex );
	critical_section(t);
	pthread_mutex_unlock( &mutex );

	printf("THREAD %d: I'm awake.\n", (int)pthread_self());

	/*dynamically allocate space to hold a return value*/
	unsigned int* x = malloc(sizeof(unsigned int));
	*x = pthread_self(); /*return the thread ID of the child*/

	pthread_exit(x);//these two things are the same
	return(x);//these two things are the same
	//return NULL; //same as pthread_exit(NULL);
}