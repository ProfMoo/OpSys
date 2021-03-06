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

#define BUFFER_SIZE 1024

pthread_mutex_t directory = PTHREAD_MUTEX_INITIALIZER;

int putErrorCheck(int newsd, char* filename) {
	if ( access(filename, F_OK) == -1 ) {

	}
	else {
		printf("[child %lu] Sent ERROR FILE EXISTS\n", pthread_self());
		fflush(NULL);
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
	int n = send( newsd, "ACK\n", 4, 0 );

	if (n != 4) {
		perror("ERROR. ACK DIDNT SEND");
		_exit(1);
	}

	printf("[child %lu] Sent ACK\n", pthread_self());
	fflush(NULL);

	return NULL;
}

void* put(int newsd, char* filenamePut, int bytes, char* fileContents) {
	//def.txt has 1139 bytes
	FILE* f = fopen(filenamePut, "a");

	printf("fileContents: %s\n", fileContents);
	fflush(NULL);
	//fseek(f, 0, SEEK_END);
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    _exit(1);
	}
	fprintf(f, "%s", fileContents);
	fclose(f);

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
		fflush(NULL);
		int n = send( newsd, "ERROR NO SUCH FILE\n", 19, 0 );
		if ( n != 19 ) {
			perror( "send() failed" );
			_exit(1);
		}
		return 1;
	}
	return 0;
}

void* get(int newsd, char* filenameGet, char* byteOffset, char* length) {
	char filename[64];
	strcpy(filename, "storage/");
	strcat(filename, filenameGet);

	if (getErrorCheck(newsd, filename, byteOffset, length)) {
		return NULL;
	}

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
		printf("[child %lu] Sent ERROR INVALID BYTE RANGE\n", pthread_self());
		fflush(NULL);
		int n = send( newsd, "ERROR INVALID BYTE RANGE\n", 25, 0 );
		if ( n != 25 ) {
			perror( "send() failed" );
			_exit(1);
		}
		return NULL;
	}

	rewind(fileptr);					// Jump back to the beginning of the file

	//making the ACK in here
	int intLength = strlen(length);
	int intSent = 6 + intLength;
	char* toSend = (char*)calloc(intSent, sizeof(char));
	strcat(toSend, "ACK ");
	strcat(toSend, length);
	toSend[intSent-2] = '\n';

	printf("[child %lu] Sent %s", pthread_self(), toSend);
	fflush(NULL);
	//done making ACK

	buffer = (char *)malloc((lengthInt+1)*sizeof(char));
	
	int fileFd = fileno(fileptr);
	ssize_t numRead = pread(fileFd, buffer, lengthInt, byteOffsetInt);

	if (numRead < 0) {
		perror("Error, file not read. pread() failed");
		_exit(1);
	}
	
	int totalLength = lengthInt+1+intSent;
	char totalBuff[totalLength];
	strcat(totalBuff, toSend);
	strcat(totalBuff, buffer);

	int n = send( newsd, totalBuff, totalLength-1, 0 );

	if ( n != totalLength-1 ) {
		perror( "send() failed" );
		_exit(1);
	}

	printf("[child %lu] Sent %s bytes of \"%s\" from offset %s\n", pthread_self(), length, filenameGet, byteOffset);
	fflush(NULL);

	fclose(fileptr); // Close the file
	free(buffer);
	free(toSend);

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
	int lengthMalloc = toSendLength+4+(numFiles-1);
	char* toSend = (char*)calloc(lengthMalloc, sizeof(char));
	strcat(toSend, strNum);
	while (i < numFiles) {
		fflush(NULL);
		strcat(toSend, filesList[i]);
		if (i != (numFiles-1)) {
			strcat(toSend, " ");
		}
		i += 1;
	}
	toSend[lengthMalloc-2] = '\n';

	int sendN = send(newsd, toSend, lengthMalloc, 0 );

	if ( sendN != lengthMalloc ) {
		perror( "send() failed" );
		//_exit(1);
	}

	printf("[child %lu] Sent %s", pthread_self(), toSend);
	fflush(NULL);

	free(toSend);
	i = 0;
	while (i < numFiles) {
		free(filesList[i]);
		i += 1;
	}

	chdir("..");
    closedir(d);
    return NULL;
}

char* wordGet(int* i, char* message, int numAlloc) {
	char* singleWord = (char*)calloc(numAlloc, sizeof(char));
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

char* wordGetPut(char** fileContents, int* i, char* message, int maxLength, int* currentRun) {
	int j = 0;
	(*fileContents)[0] = message[(*i)];
	while(1) {
		j += 1;
		if ((*currentRun) == maxLength) {
			break;
		}
		else {
			(*fileContents)[j] = message[(*i)+j]; 
			(*currentRun)++;
		}
	}

	(*i) += j;
	return(*fileContents);
}

void* handleMessage(char* message, int newsd) {
	int counter = 0;
	if (message[0] == 'P') {
		//PUT <filename> <bytes>\n<file-contents>
		char* putCommand;
		char* filenamePut;
		char* bytes;
		putCommand = wordGet(&counter, message, 81);
		filenamePut = wordGet(&counter, message, 81);
		bytes = wordGet(&counter, message, 81);
		putCommand[80] = '\0';
		filenamePut[80] = '\0';
		bytes[80] = '\0';

		//this is the total amount of bytes needed to be stored
		int bytesInt = (int)strtol(bytes, (char**)NULL, 10);

		char filenameP[64];
		strcpy(filenameP, "storage/");
		strcat(filenameP, filenamePut);

		if (putErrorCheck(newsd, filenameP)) {
			return NULL;
		}

		int numPerScan = 1024;
		char* fileContents = (char*)calloc(numPerScan+1, sizeof(char));

		pthread_mutex_lock(&directory);
		int numThisScan;
		counter++;
		//counter++;
		while (bytesInt > numPerScan) { //while the amount of bytes in the file is greater than the amount per scan
			numThisScan = 0;
			fileContents = wordGetPut(&fileContents, &counter, message, numPerScan, &numThisScan);
			fileContents[numThisScan+1] = '\0';
			//printf("bytesInt2: %d\n", bytesInt);
			//printf("fileContents2: %s\n", fileContents);
			fflush(NULL);
			put(newsd, filenameP, bytesInt, fileContents);
			//printf("bytesInt2a: %d\n", bytesInt);
			bytesInt -= numThisScan;
			//printf("bytesInt2b: %d\n", bytesInt);
			fflush(NULL);
		}
		numThisScan = 0;
		fileContents = wordGetPut(&fileContents, &counter, message, numPerScan, &numThisScan);
		fileContents[numThisScan+1] = '\0';
		//printf("bytesInt1: %d\n", bytesInt);
		//printf("fileContents1: %s\n", fileContents);
		fflush(NULL);
		put(newsd, filenameP, bytesInt, fileContents);
		pthread_mutex_unlock(&directory);

		//printf("fileContents: %s\n", fileContents);

		printf("[child %lu] Received PUT %s %s\n", pthread_self(), filenamePut, bytes);
		fflush(NULL);

		printf("[child %lu] Stored file \"%s\" (%s bytes)\n", pthread_self(), filenamePut, bytes);
		fflush(NULL);

		putSendAck(newsd);

		free(putCommand);
		free(filenamePut);
		free(bytes);
		free(fileContents);
	}
	else if (message[0] == 'G') {
		//GET <filename> <byte-offset> <length>\n
		char* getCommand;
		char* filenameGet;
		char* byteOffset;
		char* length;
		getCommand = wordGet(&counter, message, 81);
		filenameGet = wordGet(&counter, message, 81);
		byteOffset = wordGet(&counter, message, 81);
		length = wordGet(&counter, message, 81);
		getCommand[80] = '\0';
		filenameGet[80] = '\0';
		byteOffset[80] = '\0';
		length[80] = '\0';

		printf("[child %lu] Received GET %s %s %s\n", pthread_self(), filenameGet, byteOffset, length);
		fflush(NULL);

		pthread_mutex_lock(&directory);
		get(newsd, filenameGet, byteOffset, length);
		pthread_mutex_unlock(&directory);

		free(getCommand);
		free(filenameGet);
		free(byteOffset);
		free(length);
		
	}
	else if (message[0] == 'L') {
		//LIST\n
		char* listCommand;
		listCommand = wordGet(&counter, message, 81);
		listCommand[80] = '\0';

		printf("[child %lu] Received LIST\n", pthread_self());
		fflush(NULL);

		pthread_mutex_lock(&directory);
		list(newsd);
		pthread_mutex_unlock(&directory);
		
		free(listCommand);
	}
	else if (message[0] == 'E') {

		_exit(EXIT_SUCCESS);
	}
	// else {
	// 	perror("bad message, son");

	// 	_exit(0);
	// }
	return NULL;
}

void* threadCall(void* arg) {
    int newsd = *((int *) arg);	
    int n;
	char buffer[ BUFFER_SIZE ];

	do {
		n = recv( newsd, buffer, BUFFER_SIZE, 0 );

		if ( n == -1 ) {
			printf("[child %lu] Client disconnected\n", pthread_self());
			fflush(NULL);
			close(newsd);
			_exit(1);
		}
		else if ( n == 0 ) {
			printf("[child %lu] Client disconnected\n", pthread_self());
			fflush(NULL);
			close(newsd);
		}
		else {
			buffer[n] = '\0';    /* assume this is text data */
			//printf("buffer: %s\n", buffer);
			//printf("strlen(buffer): %lu\n", strlen(buffer));
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