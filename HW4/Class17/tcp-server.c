/* tcp-server.c */

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

/* 

TO CONNECT:
telnet MACHINE PORTNUM

*/

int main()
{
  /* Create the listener socket as TCP socket */
  int sd = socket( PF_INET, SOCK_STREAM, 0 );
    /* here, the sd is a socket descriptor (part of the fd table) */

  if ( sd == -1 )
  {
    perror( "socket() failed" );
    return EXIT_FAILURE;
  }


  /* socket structures */
  struct sockaddr_in server;
  server.sin_family = PF_INET; /*Could be AF_INET. It's identical for our purposes*/
  server.sin_addr.s_addr = INADDR_ANY;  /* allow any IP address to connect */

  unsigned short port = 8123;

  /* htons() is host-to-network short for data marshalling */
  /* Internet is big endian; Intel is little endian */
  server.sin_port = htons( port );
  int len = sizeof( server );

  /* bind to a specific port */
  if ( bind( sd, (struct sockaddr *)&server, len ) == -1 )
  {
    perror( "bind() failed" );
    return EXIT_FAILURE;
  }


  /* identify this port as a listener port */
  /* the 5 means that we will accept 5 clients in a backlog. We can queue up 5 connections,
  but hte 6th one will be rejected*/
  if ( listen( sd, 5 ) == -1 )
  {
    perror( "listen() failed" );
    return EXIT_FAILURE;
  }

  printf( "SERVER: Listener bound to port %d\n", port );

  struct sockaddr_in client;
  int fromlen = sizeof( client );

  int n;
  char buffer[ BUFFER_SIZE ];

  while ( 1 )
  {
    printf( "SERVER: Blocked on accept()\n" );
    int newsd = accept( sd, (struct sockaddr *)&client, (socklen_t *)&fromlen );

    printf( "SERVER: Accepted new client connection on sd %d\n", newsd );

    /*we have just established a TCP connection between server and a remote client;
    below implements the application-layer protocol*/

    /*the sd variable is the listener that we use to accept new incoming client connections (port 8123)*/
    /*the newsd variable is the means of communicating (recv()/send()) with the already connected (ex: port 41234)*/

    do
    {
      printf( "SERVER: Blocked on recv()\n" );

      /* recv() will block until we receive data (n > 0)
          or there's an error (n == -1)
           or the client closed the socket (n == 0) */
      n = recv( newsd, buffer, BUFFER_SIZE, 0 );

      if ( n == -1 )
      {
        perror( "recv() failed" );
        return EXIT_FAILURE;
      }
      else if ( n == 0 )
      {
        printf( "SERVER: Rcvd 0 from recv(); closing socket...\n" );
      }
      else /* n > 0 */
      {
        buffer[n] = '\0';    /* assume this is text data */
        printf( "SERVER: Rcvd message from %s: %s\n",
                inet_ntoa( (struct in_addr)client.sin_addr ),
                buffer );

        /* send ACK message back to the client */
        n = send( newsd, "ACK\n", 4, 0 );

        if ( n != 4 )
        {
          perror( "send() failed" );
          return EXIT_FAILURE;
        }
      }
    }
    while ( n > 0 );

    close( newsd );
  }

  return EXIT_SUCCESS;
}