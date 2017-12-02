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

//#include "hw4.h"

#define BUFFER_SIZE 1024

int putErrorCheck(int newsd, char* filename, char* bytes, char* fileContents) {
	if ( access(filename, F_OK) == -1 ) {

	}
	else {
		perror("ERROR FILE EXISTS\n");
		int n = send( newsd, "ERROR FILE EXISTS\n", 18, 0 );
		if ( n != 18 ) {
			perror( "send() failed" );
			_exit(1);
		}
		return 1;
	}
	return 0;
}

void* putSendAck(int newsd) {
	char* toSend = (char*)calloc(4, sizeof(char));
	strcat(toSend, "ACK");
	toSend[3] = '\n';

	int n = send( newsd, toSend, 4, 0 );

	if (n != 4) {
		perror("ERROR. ACK DIDNT SEND");
		_exit(1);
	}

	printf("[child %lu] Sent ACK\n", pthread_self());
	fflush(NULL);
	free(toSend);
	return NULL;
}

void* put(int newsd, char* filenamePut, char* bytes, char* fileContents) {
	//def.txt has 1139 bytes
	putErrorCheck(newsd, filenamePut, bytes, fileContents);

	putSendAck(newsd);
	return NULL;
}

int getErrorCheck(int newsd, char* filename, char* byteOffset, char* length) {
	//the user needs to enter digits
	if (!isdigit(byteOffset[0])) {
		perror("Byteoffset needs to be a number");
		return 1;
	}
	if (!isdigit(length[0])) {
		perror("Length needs to be a number");
		return 1;
	}
	//letting the user know of a bad filename and exits
	if ( access(filename, F_OK) == -1 ) {
		printf("[child %lu] Sent ERROR NO SUCH FILE\n", pthread_self());
		int n = send( newsd, "ERROR NO SUCH FILE\n", 19, 0 );
		if ( n != 19 ) {
			perror( "send() failed" );
			_exit(1);
		}
		return 1;
	}
	return 0;
}

void* getSendAck(int newsd, char* length) {
	int intLength = strlen(length);
	char* toSend = (char*)calloc(5 + intLength, sizeof(char));
	strcat(toSend, "ACK ");
	strcat(toSend, length);
	toSend[4+intLength] = '\n';

	int n = send( newsd, toSend, 5 + intLength, 0 );

	if (n != 5 + intLength) {
		perror("ERROR. ACK DIDNT SEND");
		_exit(1);
	}

	printf("[child %lu] Sent %s", pthread_self(), toSend);
	fflush(NULL);
	free(toSend);
	return NULL;
}

void* get(int newsd, char* filenameGet, char* byteOffset, char* length) {
	char filename[64];
	strcpy(filename, "storage/");
	strcat(filename, filenameGet);

	if (getErrorCheck(newsd, filename, byteOffset, length)) {
		return NULL;
	}

	getSendAck(newsd, length);

	int byteOffsetInt = (int)strtol(byteOffset, (char**)NULL, 10);
	int lengthInt = (int)strtol(length, (char**)NULL, 10);

	FILE *fileptr;
	char *buffer;
	long filelen;

	fileptr = fopen(filename, "rb");	// Open the file in binary mode
	fseek(fileptr, 0, SEEK_END);		// Jump to the end of the file
	filelen = ftell(fileptr);			// Get the current byte offset in the file

	//convert byteoffset and length, add and see if longer than file
	if (byteOffsetInt + lengthInt > filelen) {
		int n = send( newsd, "ERROR INVALID BYTE RANGE\n", 25, 0 );
		if ( n != 25 ) {
			perror( "send() failed" );
			_exit(1);
		}
		return NULL;
	}

	rewind(fileptr);					// Jump back to the beginning of the file

	buffer = (char *)malloc((lengthInt+1)*sizeof(char));
	
	int fileFd = fileno(fileptr);
	ssize_t numRead = pread(fileFd, buffer, lengthInt, byteOffsetInt);

	if (numRead < 0) {
		perror("Error, file not read. pread() failed");
		_exit(1);
	}
	
	buffer[lengthInt] = '\n';
	int n = send( newsd, buffer, lengthInt+1, 0 );

	if ( n != lengthInt+1 ) {
		perror( "send() failed" );
		_exit(1);
	}

	printf("[child %lu] Sent %s bytes of \"%s\" from offset %s\n", pthread_self(), length, filenameGet, byteOffset);
	fflush(NULL);

	fclose(fileptr); // Close the file
	free(buffer);

	return NULL;
}

void* list(int newsd) {
	int numFiles=0;
	int i=0;
	unsigned long toSendLength = 0;
	DIR *d;
	struct dirent *dir;
	d = opendir("storage");

	int ret;
	ret = chdir("storage");

	if (ret == -1) {
		perror( "chdir() failed" );
	}

	//Determine the number of files
	while((dir = readdir(d)) != NULL) {
	    if ( !strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..") )
	    {

	    } else {
	        numFiles++;
	    }
	}
	//printf("numFiles: %d\n", numFiles);
	rewinddir(d);

	char *filesList[numFiles];

	//Put file names into the array
	while((dir = readdir(d)) != NULL) {
	    if ( !strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..") )
	    {}
	    else {
	    	filesList[i] = (char*)calloc(strlen(dir->d_name)+1, sizeof(char));
	        strncpy (filesList[i], dir->d_name, strlen(dir->d_name) );
	        //printf("length: %lu\n", strlen(dir->d_name));
	        toSendLength += strlen(dir->d_name);
	        i++;
	    }
	}
	rewinddir(d);

	char strNum[3];
	sprintf(strNum, "%d ", numFiles);

	i = 0;
	char* toSend = (char*)calloc(toSendLength+3, sizeof(char));
	strcat(toSend, strNum);
	while (i < numFiles) {
		strcat(toSend, filesList[i]);
		i += 1;
	}
	toSend[toSendLength+2] = '\n';

	int sendN = send(newsd, toSend, toSendLength+3, 0 );

	if ( sendN != toSendLength+3 ) {
		perror( "send() failed" );
		//_exit(1);
	}

	printf("[child %lu] Sent %s", pthread_self(), toSend);

	chdir("..");
    closedir(d);
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

		printf("[child %lu] Received PUT %s %s\n", pthread_self(), filenamePut, fileContents);
		put(newsd, filenamePut, bytes, fileContents);
		
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

		printf("[child %lu] Received GET %s %s %s\n", pthread_self(), filenameGet, byteOffset, length);
		get(newsd, filenameGet, byteOffset, length);

		free(getCommand);
		free(filenameGet);
		free(byteOffset);
		free(length);
		
	}
	else if (message[0] == 'L') {
		//LIST\n
		char* listCommand = (char*)calloc(80, sizeof(char));
		listCommand = wordGet(&counter, message);

		printf("[child %lu] Received LIST\n", pthread_self());
		list(newsd);
		
		free(listCommand);
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
    int n;
	char buffer[ BUFFER_SIZE ];

	do {
		n = recv( newsd, buffer, BUFFER_SIZE, 0 );

		if ( n == -1 ) {
			perror( "recv() failed" );
			_exit(1);
		}
		else if ( n == 0 ) {
		}
		else {
			buffer[n] = '\0';    /* assume this is text data */
			handleMessage(buffer, newsd);
		}
	}
	while ( n > 0 );
    pthread_detach(pthread_self());
    return NULL;
}

int startConnection(char* portString) {
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


	unsigned short port = (unsigned short)strtol(portString, (char**)NULL, 10);
	//unsigned short port = 8123;

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

	printf("Started server\n");
	printf("Listening for TCP connection on port: %d\n", port);
	fflush(NULL);
	struct sockaddr_in client;
	int fromlen = sizeof( client );

	// int n;
	pthread_t tid;
	// char buffer[ BUFFER_SIZE ];

	while ( 1 ) {
		int newsd = accept( sd, (struct sockaddr *)&client, (socklen_t *)&fromlen );

		printf( "Rcvd incoming TCP connection from %s\n", inet_ntoa( (struct in_addr)client.sin_addr ) );
		fflush(NULL);

		int rc = pthread_create( &tid, NULL, threadCall, &newsd);
		if (rc < 0) {
			perror("pthread_create() failed");
			_exit(1);
		}
	}

	return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {	
	//check for correct input here

	if (argc != 2) {
		perror("USAGE BAD. CORRECT IS ./a.out <port-number>");
		_exit(1);
	}

	startConnection(argv[1]);
}