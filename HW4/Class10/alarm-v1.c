#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINE 80

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
			fprintf( stderr, "ERROR: invalud alarm request\n" );
		}
		else
		{
			printf( "Alarm set for %d seconds\n", seconds);
			sleep(seconds);
			printf( "Alarm (%d): %s\n", seconds, msg);
		}

	}

	return EXIT_SUCCESS;
}
