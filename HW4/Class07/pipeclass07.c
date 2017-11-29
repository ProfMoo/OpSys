/* pipe-ipc.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  int p[2];   /* array to hold the two pipe (file) descriptors:
                 p[0] is the read end of the pipe
                 p[1] is the write end of the pipe */

  int rc = pipe( p );  /* the input arg p will be filled in
                           by the pipe() call... */

  if ( rc == -1 )
  {
    perror( "pipe() failed" );
    return EXIT_FAILURE;
  }

  /* fd table:

     0: stdin
     1: stdout
     2: stderr                        +--------+
     3: p[0] <=========READ========== | buffer | think of this buffer as
     4: p[1] ==========WRITE========> | buffer |  a temporary hidden file...
                                      +--------+
  */

  printf( "Created pipe; p[0] is %d and p[1] is %d\n", p[0], p[1] );


  pid_t pid = fork();  /* this will copy the fd table from parent to child */

  if ( pid == -1 )
  {
    perror( "fork() failed" );
    return EXIT_FAILURE;
  }

  /* fd table:

     [PARENT]                                       [CHILD]
     0: stdin                                       0: stdin
     1: stdout                                      1: stdout
     2: stderr                +--------+            2: stderr
     3: p[0] <=====READ=====< | buffer | >==READ==> 3: p[0]
     4: p[1] >=====WRITE====> | buffer | <==WRITE=< 4: p[1]
                              +--------+
  */

  if ( pid == 0 )  /* CHILD */
  {
    close( p[0] );   /* close the read end of the pipe */
    p[0] = -1;

    printf( "CHILD: happy birthday to me!\n" );

    printf( "CHILD: (pretend) I couldnt write and i crashed\n");
#if 0
    /* write some data to the pipe */
    int bytes_written = write( p[1], "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26 );
    printf( "CHILD: Wrote %d bytes to the pipe\n", bytes_written );
     /* to do: check the return value from write() for error... */
#endif
  }
  else /* pid > 0     PARENT */
  {
//lets not close it, so there are multiple places to write to it
    close( p[1] );   /* close the write end of the pipe */
    p[1] = -1;

    /*if you read 0 bytes, that's telling you no one can write to the pipe. if it holds,
    then someone can write to it, it just hasn't happened yet. it's waiting*/

    printf( "PARENT: I'm about to block on read()\n" );

    /* read some data from the pipe */
    char buffer[80];
    int bytes_read = read( p[0], buffer, 10 );

    if (bytes_read == 0)
    {
      printf( "PARENT: the child process terminated without writing data \n");
    }
    else 
    {
      buffer[bytes_read] = '\0';
      printf( "PARENT: Read %d bytes: %s\n", bytes_read, buffer );
    }
  }

  /* fd table (after the close( p[0] ) and close( p[1] ) calls):

     [PARENT]                                       [CHILD]
     0: stdin                                       0: stdin
     1: stdout                                      1: stdout
     2: stderr                +--------+            2: stderr
     3: p[0] <=====READ=====< | buffer |            3:     
     4:                       | buffer | <==WRITE=< 4: p[1]
                              +--------+
  */
  return EXIT_SUCCESS;
}
