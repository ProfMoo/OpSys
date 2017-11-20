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

int x = 12;

void * lmao( void * arg )
{
	int q = 4;
	int * z = (int *)arg;
	q -= 2;
	printf( "%u %d %d\n", (unsigned int)pthread_self(), *z, q );
	*z *= 2;
	return NULL;
}

int main()
{
	printf( "%u %d\n", (unsigned int)pthread_self(), x );
	pthread_t tid1, tid2;
	pthread_create( &tid1, NULL, lmao, &x );
	pthread_create( &tid2, NULL, lmao, &x );
	pthread_join( tid1, NULL );
	pthread_join( tid2, NULL );
	printf( "%u %d\n", (unsigned int)pthread_self(), x );
	return EXIT_SUCCESS;
}