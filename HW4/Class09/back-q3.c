#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>

int main()
{
	close(2);
	char buffer[128];
	int x = 12;
	int p[2];
	int rc = pipe(p);

	pid_t pid = fork();
	printf("%d %d %d\n", pid, p[0], p[1]);

	if (pid == 0) {
		x = x/2;
		pid = write(p[1], "WHOOPS", x);
		printf("%d WROTE DATA\n", getpid() );
	}

	if (pid > 0) {
		x = x/3;
		printf( "%d READING... \n", pid );
		rc = read( p[0], buffer, x);
		buffer[rc] = '\0';
		printf( "%d %s \n", getpid(), buffer);
	}

	printf( "%d %d\n", getpid(), x);

	return EXIT_SUCCESS;
}