/* udp-server.c */

/* To test this server, you can use the following
   command-line netcat tool:

   bash$ netcat -u linux00.cs.rpi.edu 41234

   The hostname (e.g., linux00.cs.rpi.edu) cannot be
   localhost; and the port number must match what
   the server reports.

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXBUFFER 8

int main()
{
  int sd;  /* socket descriptor ---- this is in the fd table */
  struct sockaddr_in server;
  int length;

  /* create the socket (endpoint) on the server side */
  sd = socket( AF_INET, SOCK_DGRAM, 0 );
                        /* UDP */

  if ( sd < 0 )   /* this will be part of the file descriptor table! */
  {
    perror( "socket() failed" );
    return EXIT_FAILURE;
  }

  server.sin_family = AF_INET;  /* IPv4 */
  server.sin_addr.s_addr = htonl( INADDR_ANY );

  /* specify the port number for the server */
  server.sin_port = htons( 0 );  /* a 0 here means let the kernel assign
                                    us a port number to listen on */

  /* bind to a specific (OS-assigned) port number */
  if ( bind( sd, (struct sockaddr *) &server, sizeof( server ) ) < 0 )
  {
    perror( "bind() failed" );
    return EXIT_FAILURE;
  }

  length = sizeof( server );

  if ( getsockname( sd, (struct sockaddr *) &server, (socklen_t *) &length ) < 0 )
  {
    perror( "getsockname() failed" );
    return EXIT_FAILURE;
  }

  printf( "UDP server at port number %d\n", ntohs( server.sin_port ) );


  /* the code below implements the application protocol */
  int n;
  char buffer[ MAXBUFFER ];
  struct sockaddr_in client;
  int len = sizeof( client );

  while ( 1 )
  {
    /* read a datagram from the remote client side (BLOCKING) */
    n = recvfrom( sd, buffer, MAXBUFFER, 0, (struct sockaddr *) &client,
                  (socklen_t *) &len );

    if ( n < 0 )
    {
      perror( "recvfrom() failed" );
    }
    else
    {
      printf( "Rcvd datagram from %s port %d\n",
              inet_ntoa( client.sin_addr ), ntohs( client.sin_port ) );

      printf( "RCVD %d bytes\n", n );
      buffer[--n] = '\0';
      printf( "RCVD: [%s]\n", buffer );

      /* echo the data back to the sender/client */
      sendto( sd, buffer, n, 0, (struct sockaddr *) &client, len );
        /* to do: check the return code of sendto() */
    }
  }

  return EXIT_SUCCESS;
}
