#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
	pid = fork();

	if (pid == -1 )
	{
		perror( "fork() failed" );
		return EXIT_FAILURE;
	}

	/*if we get here, then fork worked and we're returning from fork() twice,
	once in the parent, once in the child */

	if (pid == 0) //we're in the child process
	{
		printf( "CHILD: happy birthday!\n" );
		int mypid = getpid(); //this is a system call to get the running process's pid value
		printf( "CHILD: my pid is %d\n", mypid );
		fflush(NULL);
	}
	else //pid > 0, in parent
	{
		printf( "PARENT: my child process has pid %d\n", pid);
		int mypid = getpid();
		printf( "PARENT: my pid is %d\n", mypid);
		fflush(NULL);
	}

	return EXIT_SUCCESS;
}