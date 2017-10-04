#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{
	/*
	in terminal:

	ps ----> see all processes running in directory
	ps -ef | less ----> see all processes running in the whole machine


	pid_t pid; // -> is process id (unsigned integer)
	*/

	//create a new child process//
	pid_t pid;

	printf( "Before fork()\n" );

	pid = fork();

	if (pid == -1 )
	{
		perror( "fork() failed" );
		return EXIT_FAILURE;
	}

	printf(" After fork()\n"); /*this will output twice*/

	/*if we get here, then fork worked and we're returning from fork() twice,
	once in the parent, once in the child */

	if (pid == 0) //we're in the child process
	{
		printf( "CHILD: happy birthday!\n" );
		int mypid = getpid(); //this is a system call to get the running process's pid value
		printf( "CHILD: my pid is %d\n", mypid );
		//sleep(5);
		return 12; //this number size if limited for reading out of the parent. cant be too large
		
	}
	else //pid > 0, in parent
	{
		printf( "PARENT: my child process has pid %d\n", pid);
		int mypid = getpid();
		printf( "PARENT: my pid is %d\n", mypid);
		
		printf( "PARENT: waiting for child to terminate... \n" );

		int status;
		#if 0
		pid_t child_pid = wait( &status ); /* wait() blocks indefinitely, until the 
											child process changes state. this waits for 
											a status update of any child, then puts the child_pid
											in the return value */
		#endif
		//pid_t child_pid = waitpid( pid, &status, 0); //now we are saying wait for a specific child
		pid_t child_pid = waitpid( -1, &status, 0); //this is essentially the same as the one without
													//a specific pid.

		printf( "PARENT: child %d terminated.. \n", child_pid);

		if ( WIFSIGNALED( status ) ) /*child process was terminated due to a signal */
		{									/* (e.g. segfault) */
			printf( "..abnormally\n" );
		}
		else if (  WIFEXITED( status ) ) /*child process returned or called exit() */
		{
			int child_return_value = WEXITSTATUS( status );
			printf( "...successfully with exit status %d\n", child_return_value);
		}
	}
	return EXIT_SUCCESS;
}