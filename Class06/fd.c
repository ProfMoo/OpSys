/* fd.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
  char name[80];
  strcpy( name, "testfile.txt" );

  printf("output?\n");

  close(1); /*close stdout*/

  	/*current fd table: fd
  						0 stdin
  						1
  						2 stderr
    */

    /*attempt to open this file: 
	O_WRONLY for writing
	O_CREAT create the file if necessary
	O_TRUNC truncate the file if it already exists, which erases file contents
	(without O_TRUNC, (over) writing starts a byte 0
	check out O_APPEND in the man page*/

  printf("output?\n"); //shouldn't come out

  int fd = open( name, O_RDONLY | O_CREAT | O_TRUNC, 0660 );
  									/*leading 0 here ^^ means its an octal number
  										0660 => 110 110 000
  												rwx rwx rwx

  										(file owner), (group permissions), (global)*/



  if ( fd == -1 )
  {
    perror( "open() failed" );
    return EXIT_FAILURE;
  }

  /*    fd table:
        0 stdin
        1 testfile.txt
        2 stderr
  */

  printf( "fd is %d\n", fd ); /*the '\n' chraracter here will flush the stdout buffer
  								if we're printing to the terminal.

  								if going to a file, then the '\n' will not flush the buffer */

  fflush(stdout);

  char buffer[80];

	/* Set j to different values here to see what
    happens if you read from a file multiple times
     (also adjust how much data is in testfile.txt) */
  sprintf( buffer, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

  int rc = write(fd, buffer, 18); //this fails for some reason
  /*also check for error here*/

  printf( "wrote %d bytes to the file\n", rc );

  close( fd );   /* remove entry 1 from the fd table */
  /*the close does not automatically flush the buffer here*/

  return EXIT_SUCCESS;
}