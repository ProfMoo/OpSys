/* signal-input.c */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

//run this, then try to hit CTRL-C

void signal_handler( int n )
{
	printf("Rcvd signal %d\n", n);

	if (n == SIGINT)
	{
		printf("Stop hitting CTRL-C!\n");
	}
	else if (n == SIGUSR1)
	{
		printf("Reloading the config file...\n");
	}
}

int main()
{
	signal( SIGINT, SIG_IGN ); /*ignore SIGINT, keyboard interrupts (CTRL-C)*/

#if 0
	signal( SIGINT, SIG_DFL ); /*set back to normal*/
#endif

	//use ps -ef | grep drmoo
	//then kill -2 PROCESSNUM
	//it will be ignored

	/* redefine SIGINT to call signal_handler() */
	signal( SIGINT, signal_handler ); /*these are assignments. so now, if it gets SIGINT, it sends it somewhere else*/
	signal( SIGUSR1, signal_handler );

	//kill -NUM PROCESSNUM 
	//isn't really kill. it just send a signal number

	//signals can interrupt sleep!!

	char name[100];
	printf("Enter your name: ");
	scanf("%s", name);
	printf("Hi %s\n", name);

	return EXIT_SUCCESS;
}