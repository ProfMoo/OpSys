#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*implementation using multiple threads (Pthreads)*/

#include <pthread.h> // <=========================
/*gcc -Wall -Werror alarm-v3.c -pthread*/

#define MAX_LINE 80

/*a multithreaded process is a single process that has a "main thread"
of execution and other "threads of execution" that all SHARE the
encompassing process's address space, variables, fd table, 
instructions to execute, etc*/

/*definition of a struct to send to each thread
(because the thread function can only accept one arg)*/
typedef struct 
{
	int seconds;
	char msg[MAX_LINE];
} alarm_t;

/*function executed by thread*/
void* alarm_thread_function( void* arg ) /* void* is just a placeholder, could be any points */
{
	alarm_t* alarm = (alarm_t*)arg;

	pthread_t mytid = pthread_self(); /*get my thread ID*/

	/* pthread_detach() enables pthreads to reclaim the thread
		resources when this thread terminated
		i.e., do not leave a "zombie thread" behind*/
	int rc = pthread_detach(mytid);
	/*TO DO: check rc for error from pthread_detach()*/

	printf( "Alarm set for %d seconds\n", alarm->seconds);
	sleep(alarm->seconds);
	printf( "Alarm (%d): %s\n", alarm->seconds, alarm->msg);

	free(alarm);

	return NULL;
	/*the thread terminates here...*/
}

int main()
{
	char line[MAX_LINE];

	int seconds;
	char msg[MAX_LINE];

	while (1)
	{
		printf("Set alarm (<sec> <msg>): " );

		if (fgets(line, MAX_LINE, stdin) == NULL)
		{
			return EXIT_FAILURE;
		}

		if (strlen(line) <= 1) continue; /*skip blank lines*/

		/*dynamically allocate memory for the next alarm*/
		alarm_t* alarm = malloc(sizeof(alarm_t));

		if (sscanf(line, "%d %[^\n]", &alarm->seconds, alarm->msg) < 2 || alarm->seconds < 0)
		{
			fprintf( stderr, "ERROR: invalid alarm request\n" );
			free(alarm);
		}
		else
		{
			pthread_t tid; /*thread id, which is similar to process ID*/

			int rc = pthread_create( &tid, NULL, alarm_thread_function, alarm);
											/*   ^^^^^^^^^^^^^^^^^^^^^  ^^^^^
													|						|
												the function each thread 	|
												will run once the thread 	|
												is up and running			|
																		pointer to the input
																		to the thread function*/

			if ( rc != 0 )
			{
				fprintf(stderr, "ERROR: pthread_create() failed (%d): %s\n", rc, strerror(rc));
				return EXIT_FAILURE;
			}

			usleep(10000);
		}

	}

	return EXIT_SUCCESS;
}
