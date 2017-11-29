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

#define SNAPCHAT 2

void * rofl( void * arg )
{
	int * f = (int *)arg;
	printf( "%uAA%d\n", (unsigned int)pthread_self(), *f );
	fprintf( stderr, "%uBB\n", (unsigned int)pthread_self() );
	return NULL;
}

int main()
{
	close( SNAPCHAT );

	printf( "%uCC\n", (unsigned int)pthread_self() );
	fprintf( stderr, "%uDD\n", (unsigned int)pthread_self() );

	int fd = open( "EE.txt", O_WRONLY | O_CREAT | O_TRUNC, 0660 );

	printf( "%uFF\n", (unsigned int)pthread_self() );
	fprintf( stderr, "%uGG%d\n", (unsigned int)pthread_self(), fd );
	
	pthread_t tid1, tid2;
	
	int rc = pthread_create( &tid1, NULL, rofl, &fd );
	rc = pthread_create( &tid2, NULL, rofl, &rc );
	
	pthread_join( tid1, NULL );
	pthread_join( tid2, NULL );
	
	printf( "%uHH\n", (unsigned int)pthread_self() );
	fprintf( stderr, "%uII\n", (unsigned int)pthread_self() );
	
	fflush( NULL );
	close( fd );
	
	return EXIT_SUCCESS;
}