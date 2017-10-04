#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>

#if 0
	file descriptor (fd)
	--each process has a table associated with it that keeps track of oits open files (aka byte streams).

	fd			C++		Java			C
	0 stdin		cin		System.in 		scanf(), read(), getchar(), ...
	1 stdout	cout	System.out		printf(), write(), putchar(), ..
	2 stderr	cerr	System.err		fprintf( stderr, "ERROR: SOMETHING\n");
										perror( "open() failed" );

	stdout and stderr (by default) are both displayed on the terminal

	stdout is buffered output
		(therefore, we use '\n' at the end of printf() statements and also fflush() to explicitly
		flush the output buffer)

	stderr (fd 2) is not buffered. we want to see them right away
#endif

int main() {
	char name[80];
	strcpy( name, "testfile.txt"); //getting the name of the text file

	int fd = open(name, O_RDONLY); //trying to open testfile.txt

	if (fd == -1) {
		perror( "open() failed" ); //printing to the error. 
		return EXIT_FAILURE;
	}

	printf( "fd is %d\n", fd ); //now to address testfile.txt, we need to use the fd printed here

	/* fd table:
		0 stdin
		1 stdout
		2 stderr
		3 testfile.txt [0_RDONLY]
	*/

	char buffer[80];
	int rc = read( fd, buffer, 79 ); /*reads from fd 3, into buffer, reading 79 bytes at the most
										leaving room for the null '\0' character that shows the end */

	buffer[rc] = '\0'; /* given that im treating this data as a string, explicitly null-terminate this string */
	printf("rc is [%d]\n", rc); //rc gives the amount of characters read in
	printf("Here: %s\n", buffer); //hold the actual file read in
	//buffer looks like this: "this is a test\n\0" 

	//now we need to close the file descriptor
	close( fd ); //removed entry 3 from fd table. we should do this right after we are done with it

	//to handle a very large file, just make a loop and read it in parts

	return EXIT_SUCCESS;
}