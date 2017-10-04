#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	int fd;
	close( 2 );

	#if 1
	close( 1 );  /* <== add this line later.... */
	#endif

	printf( "HI\n" );
	fflush( stdout );

	fd = open( "newfile.txt", O_WRONLY | O_CREAT | O_TRUNC, 0600 );

	printf( "==> %d\n", fd );
	printf( "WHAT?\n" );
	fprintf( stderr, "ERROR\n" ); //only difference between fprintf and printf is specifying where 
								  //you are writing

	fflush(stdout); //<== added by G in class

	close( fd ); //does not automatically flush the buffer
	
	return EXIT_SUCCESS;
}