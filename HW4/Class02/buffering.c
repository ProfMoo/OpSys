/* buffering.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
	printf( "HERE3" ); /* this gets buffered because its on fd 1 stdout. no \n means no flush */

	fprintf( stderr, "ERROR: something went wrong\n" ); /*this does not get buffered because it's on the stderr */

	printf( "HERE4" ); 

	fflush( NULL ); /* flushed all of the buffers */

	//if we do ./a.out > output.txt, we see the stderr on the terminal, and the stdout in the text file

	return EXIT_SUCCESS;
}