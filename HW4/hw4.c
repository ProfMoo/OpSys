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

#include "hw4.h"

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

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

		if ( n == -1 )
		{
			perror( "recv() failed" );
			_exit(1);
		}
		else if ( n == 0 )
		{
			printf( "CHILD %d: Rcvd 0 from recv(); closing socket...\n", getpid() );
		}
		else /* n > 0 */
		{
			buffer[n] = '\0';    /* assume this is text data */
			printf( "CHILD %d: Rcvd message from %s: %s\n", getpid(), inet_ntoa( (struct in_addr)client.sin_addr ), buffer );

			/* send ACK message back to the client */
			n = send( newsd, "ACK\n", 4, 0 );

			if ( n != 4 )
			{
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

	if ( bind( sd, (struct sockaddr *)&server, len ) == -1 )
	{
		perror( "bind() failed" );
		return EXIT_FAILURE;
	}

	/* identify this port as a listener port */
	/* the value 5 here means that the OS will
	maintain a queue of at most 5
	pending connections for the accept() call */
	if ( listen( sd, 5 ) == -1 )
	{
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