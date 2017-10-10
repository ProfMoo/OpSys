#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINE 80

/*drawbacks: (1) need to handle zombie/defunct processes
			 (2) upper limit on number of alarms that can be set
			 (3) inefficient in terms of memory
*/


int main()
{
	char line[MAX_LINE];

	int seconds;
	char msg[MAX_LINE];

	while (1)
	{
		printf("Set alarm (<sec> <msg>): " );

		if (fgets(line, MAX_LINE, stdin) == NULL)
		{
			return EXIT_FAILURE;
		}

		if (strlen(line) <= 1) continue;

		if (sscanf(line, "%d %[^\n]", &seconds, msg) < 2 || seconds < 0)
		{
			fprintf( stderr, "ERROR: invalid alarm request\n" );
		}
		else
		{
			/*create a child process to create (and wait) for the alarm*/
			pid_t pid = fork();

			if (pid < 0)
			{
				perror("fork() failed");
				return EXIT_FAILURE;
			}
			else if (pid == 0) /*child*/
			{
				printf( "Alarm set for %d seconds\n", seconds);
				sleep(seconds);
				printf( "ALARM (%d): %s\n", seconds, msg);
				exit(EXIT_SUCCESS);
			}
			else /*parent*/
			{
				usleep(10000);
				/*TODOL add waitpid() with the WNOHAND option to kill zombies*/
			}
			

			// printf( "Alarm set for %d seconds\n", seconds);
			// sleep(seconds);
			// printf( "Alarm (%d): %s\n", seconds, msg);
		}

	}

	return EXIT_SUCCESS;
}
