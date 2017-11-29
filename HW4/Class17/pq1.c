#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <pthread.h>

void * omg( void * arg )
{
	int * x = (int *)arg;
	int rc = pthread_detach( pthread_self() );
	
	printf( "%u lucky %d %d\n",
	(unsigned int)pthread_self(), *x, rc );
	return NULL;
}

int main()
{
	pthread_t tid;
	int x = 7;

	printf( "%u %d\n", (unsigned int)pthread_self(), x );
	int rc = pthread_create( &tid, NULL, omg, &x );
	
	printf( "%u %d\n", (unsigned int)pthread_self(), rc );
	rc = pthread_create( &tid, NULL, omg, &x );
	//usleep( 1000 );  /* wait for child threads to end ... */
	
	return EXIT_SUCCESS;
}