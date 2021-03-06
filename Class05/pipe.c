/* pipe.c */

/* a pipe is a means of inter-process communication that is unidirectional */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	int p[2]; /*array to hold the two pipe (file) descriptors:
				p[0] is the read end of the pipe
				p[1] is the write end of the pipe */

	int rc = pipe(p); /* the input arg p will be filled in by the pipe() call.. */

	if (rc == -1) {
		perror( "pipe() failed" );
		return EXIT_FAILURE;
	}

	/* fd table:
		0: stdin
		1: stdout
		2: stderr					+--------+
		3: p[0] <===== READ ===== 	| buffer | think of this buffer as 
		4: p[1] ===== WRITE ====>	| buffer |	a temporary hidden file...
									+--------+
	*/

	printf( "Created pipe; p[0] is %d and p[1] is %d\n", p[0], p[1]);

	/* write some data to the pipe */
	int bytes_written = write( p[1], "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26); //can be any data here. 

	printf( "Wrote %d bytes to the pipe\n", bytes_written);
	/*to do: check the return value from write() for error... */

	/* read some data from the pipe */
	char buffer[80];
	int bytes_read = read( p[0], buffer, 10);
	buffer[bytes_read] = '\0';
	printf("Read %d bytes: %s\n", bytes_read, buffer);

	/*read some more data from the pipe*/
	bytes_read = read( p[0], buffer, 10);
	buffer[bytes_read] = '\0';
	printf("Read %d bytes: %s\n", bytes_read, buffer);

	bytes_read = read( p[0], buffer, 10);
	buffer[bytes_read] = '\0';
	printf("Read %d bytes: %s\n", bytes_read, buffer);

	/* if this code is uncommented, we will be hung on this. read waits, it
	is a blocking system call
	bytes_read = read( p[0], buffer, 10);
	buffer[bytes_read] = '\0';
	printf("Read %d bytes: %s\n", bytes_read, buffer);
	*/

	return EXIT_SUCCESS;
}