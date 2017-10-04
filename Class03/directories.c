#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
	DIR* dir = opendir("."); /*open the current working directory*/
	//DIR* dir = opendir("assignments") /*opens assignment directory, which is current inside working directory*/

	if (dir == NULL) {
		perror( "opendir() failed" );
		return EXIT_FAILURE;
	}

	struct dirent* file;

	while ((file = readdir(dir)) != NULL) {
		printf( "found %s", file->d_name ); //prints out the name of all files in the directory

		struct stat buf;
		int rc = lstat( file -> d_name, &buf); //look up man page for LSTAT!!!!
		
		//to change current working directory, try these:
		//we could either string concatenate -> "assignemnts/xyz.txt"
		//or we could chdir()

		if (rc == -1) {
			perror( "lstat() failed" );
			return EXIT_FAILURE;
		}

		printf( " (%d bytes)", (int)buf.st_size );

		if (S_ISREG(buf.st_mode)) {
			printf( " -- regular file \n");
		}
		else if ( S_ISDIR(buf.st_mode)) {
			printf( " -- directory \n");
		}
	}
}