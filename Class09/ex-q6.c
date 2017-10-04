#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	int rc;
	int p[2];

	rc = pipe( p );
	printf( "%d %d %d\n", getpid(), p[0], p[1] );
	rc = fork();

	if ( rc == 0 )
	{
		rc = write( p[1], "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26 );
	}
	if ( rc > 0 )
	{
		char buffer[40];
		rc = read( p[0], buffer, 8 );
		buffer[rc] = '\0';
		printf( "%d %s\n", getpid(), buffer );
	}
	
	printf( "BYE\n" );
	return EXIT_SUCCESS;
}