#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "hw4.h"

#define BUFFER_SIZE 1024

void* put(char* filenamePut, char* bytes, char* fileContents) {
	return NULL;
}

void* getErrorCheck(char* filename, char* byteOffset, char* length) {
	//the user needs to enter digits
	if (!isdigit(byteOffset[0])) {
		perror("Byteoffset needs to be a number");
		_exit(1);
	}
	if (!isdigit(length[0])) {
		perror("Length needs to be a number");
		_exit(1);
	}
	//letting the user know of a bad filename and exits
	if ( access(filename, F_OK) == -1 ) {
		perror("File doesn't exist");
		_exit(1);
	}
	return NULL;
}

void* get(char* filenameGet, char* byteOffset, char* length, int newsd) {
	char filename[64];
	strcpy(filename, "storage/");
	strcat(filename, filenameGet);
	//printf("filename: %s\n", filename);

	getErrorCheck(filename, byteOffset, length);
	int byteOffsetInt = (int)strtol(byteOffset, (char**)NULL, 10);
	int lengthInt = (int)strtol(length, (char**)NULL, 10);

	FILE *fileptr;
	char *buffer;
	long filelen;

	fileptr = fopen(filename, "rb");	// Open the file in binary mode
	fseek(fileptr, 0, SEEK_END);		// Jump to the end of the file
	filelen = ftell(fileptr);			// Get the current byte offset in the file

	//printf("filelen: %lu\n", filelen);

	//convert byteoffset and length, add and see if longer than file
	if (byteOffsetInt + lengthInt > filelen) {
		perror("Trying to access outside of file");
		_exit(1);
	}

	rewind(fileptr);					// Jump back to the beginning of the file

	buffer = (char *)malloc((lengthInt+1)*sizeof(char)); // Enough memory for file + \0
	
	int fileFd = fileno(fileptr);
	ssize_t numRead = pread(fileFd, buffer, lengthInt, byteOffsetInt);
	printf("buffer: %s\n", buffer);
	fflush(NULL);

	if (numRead < 0) {
		perror("Error, file not read. pread() failed");
		_exit(1);
	}
	
	int n = send( newsd, buffer, lengthInt, 0 );

	if ( n != lengthInt ) {
		perror( "send() failed" );
		_exit(1);
	}

	fclose(fileptr); // Close the file

	return NULL;
}

void* list(char* message) {
	return NULL;
}

char* wordGet(int* i, char* message) {
	char* singleWord = (char*)calloc(80, sizeof(char));
	int j = 0;
	for(; *i < strlen(message); (*i)++) {
		// printf("i: %d\n", *i);
		if (isalpha(message[(*i)]) || isdigit(message[(*i)])) { //begin word
			j = 0;
			singleWord[0] = message[(*i)];
			while(1) { //reading a word
				j += 1;
				if(isalpha(message[(*i)+j])) {
					singleWord[j] = message[(*i)+j]; 
				}
				else if(isdigit(message[(*i)+j])) {
					singleWord[j] = message[(*i)+j]; 
				}
				else if(message[(*i)+j] == '.') {
					singleWord[j] = message[(*i)+j];
				}
				else {
					break;
				}
			}
		}
		else {
			continue;
		}
		(*i) += j;
		return(singleWord);
	}
	return NULL;
}

void* handleMessage(char* message, int newsd) {
	int counter = 0;
	if (message[0] == 'P') {
		//PUT <filename> <bytes>\n<file-contents>
		char* putCommand = (char*)calloc(80, sizeof(char));
		char* filenamePut = (char*)calloc(80, sizeof(char));
		char* bytes = (char*)calloc(80, sizeof(char));
		char* fileContents = (char*)calloc(80, sizeof(char));
		putCommand = wordGet(&counter, message);
		filenamePut = wordGet(&counter, message);
		bytes = wordGet(&counter, message);
		fileContents = wordGet(&counter, message);

		printf("words: %s, %s, %s, %s\n", putCommand, filenamePut, bytes, fileContents);
		put(filenamePut, bytes, fileContents);
		
		free(putCommand);
		free(filenamePut);
		free(bytes);
		free(fileContents);
	}
	else if (message[0] == 'G') {
		//GET <filename> <byte-offset> <length>\n
		char* getCommand = (char*)calloc(80, sizeof(char));
		char* filenameGet = (char*)calloc(80, sizeof(char));
		char* byteOffset = (char*)calloc(80, sizeof(char));
		char* length = (char*)calloc(80, sizeof(char));
		getCommand = wordGet(&counter, message);
		filenameGet = wordGet(&counter, message);
		byteOffset = wordGet(&counter, message);
		length = wordGet(&counter, message);

		printf("words: %s, %s, %s, %s\n", getCommand, filenameGet, byteOffset, length);
		get(filenameGet, byteOffset, length, newsd);

		free(getCommand);
		free(filenameGet);
		free(byteOffset);
		free(length);
		
	}
	else if (message[0] == 'L') {
		//LIST\n
		char* list = (char*)calloc(80, sizeof(char));
		list = wordGet(&counter, message);

		printf("words: %s\n", list);
		
		free(list);
	}
	else if (message[0] == 'E') {

		_exit(0);
	}
	else {
		perror("bad message, son");

		_exit(0);
	}
	return NULL;
}

void* threadCall(void* arg) {
    int newsd = *((int *) arg);	

    printf("newsd: %d\n", newsd);

    int n;
	char buffer[ BUFFER_SIZE ];
	struct sockaddr_in client;

	do {
		printf( "CHILD %d: Blocked on recv()\n", getpid() );

		/* recv() will block until we receive data (n > 0)
		or there's an error (n == -1)
		or the client closed the socket (n == 0) */
		n = recv( newsd, buffer, BUFFER_SIZE, 0 );

		if ( n == -1 ) {
			perror( "recv() failed" );
			_exit(1);
		}
		else if ( n == 0 ) {
			printf( "CHILD %d: Rcvd 0 from recv(); closing socket...\n", getpid() );
		}
		else {
			buffer[n] = '\0';    /* assume this is text data */
			printf( "CHILD %d: Rcvd message from %s: Sending to function \"%s\"\n", getpid(), inet_ntoa( (struct in_addr)client.sin_addr ), buffer );
			handleMessage(buffer, newsd);

			/* send ACK message back to the client */
			n = send( newsd, "ACK\n", 4, 0 );

			if ( n != 4 ) {
				perror( "send() failed" );
				_exit(1);
			}
		}
	}
	while ( n > 0 );
    pthread_detach(pthread_self());
    return NULL;
}

int startConnection() {
	/* Create the listener socket as TCP socket */
	int sd = socket( PF_INET, SOCK_STREAM, 0 );
	/* here, the sd is a socket descriptor (part of the fd table) */

	if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0) {
		perror("setsockopt(SO_REUSEADDR) failed");
	}

	if ( sd == -1 ) {
		perror( "socket() failed" );
		return EXIT_FAILURE;
	}

	/* socket structures */
	struct sockaddr_in server;
	server.sin_family = PF_INET;
	server.sin_addr.s_addr = INADDR_ANY;  /* allow any IP address to connect */

	unsigned short port = 8123;

	/* htons() is host-to-network short for data marshalling */
	/* Internet is big endian; Intel is little endian */
	server.sin_port = htons( port );
	int len = sizeof( server );

	if ( bind( sd, (struct sockaddr *)&server, len ) == -1 ) {
		perror( "bind() failed" );
		return EXIT_FAILURE;
	}

	/* identify this port as a listener port */
	/* the value 5 here means that the OS will
	maintain a queue of at most 5
	pending connections for the accept() call */
	if ( listen( sd, 5 ) == -1 ) {
		perror( "listen() failed" );
		return EXIT_FAILURE;
	}

	printf( "PARENT: Listener bound to port %d\n", port );
	struct sockaddr_in client;
	int fromlen = sizeof( client );

	// int n;
	pthread_t tid;
	// char buffer[ BUFFER_SIZE ];

	while ( 1 ) {
		printf( "PARENT: Blocked on accept()\n" );
		int newsd = accept( sd, (struct sockaddr *)&client, (socklen_t *)&fromlen );

		printf( "PARENT: Accepted new client connection on sd %d\n", newsd );

		int rc = pthread_create( &tid, NULL, threadCall, &newsd);
		printf("MAIN: Created child thread for \"%d\" and printing rc: %d \n", newsd, rc);

		/* Handle the accepted new client connection in a child thread */
	}

	return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {	
	//check for correct input here

	startConnection();
}