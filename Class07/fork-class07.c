/* fork.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  pid_t pid;   /* process id (unsigned integer) */

  /* create a new child process */
  pid = fork();
    /* consider renaming the pid variable here to rc */

  if ( pid == -1 )
  {
    perror( "fork() failed" );
    return EXIT_FAILURE;
  }


  /* if we get here, then fork() worked and we're returning from
      fork() twice, once in the parent, once in the child */

  if ( pid == 0 )  /* CHILD */
  {
    printf( "CHILD: happy birthday!\n" );
    int mypid = getpid();   /* getpid() is a system call to get the
                                running process's pid value */
    printf( "CHILD: my pid is %d\n", mypid );

    /*in this child process memory space, execute the /bin/ls executable;
    use "ls" as argv[0], "-a" as argv[1]. (argc == 2) */
    execlp( "/bin/ls", "ls", "-a", NULL); //this cancels the whole process and overwrite it with this
    /*to call self:
    execlp( "./a.out", "a.out", NULL); //is a fork bomb! careful
    */

    /*we should never get here (unless execvp() failed))*/

    perror("execlp() failed");
    return EXIT_FAILURE;
  }
  else /* pid > 0     PARENT */
  {
    printf( "PARENT: my child process has pid %d\n", pid );
    int mypid = getpid();
    printf( "PARENT: my pid is %d\n", mypid );
    sleep(5);
    /*to do: add the wait() or waitpid() call...*/
  }

  return EXIT_SUCCESS;
}
