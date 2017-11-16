/* server-select.c */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include <sys/select.h>      /* <===== */

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100      /* <===== */


int main()
{
  /* ====== */
  fd_set readfds;
  int client_sockets[ MAX_CLIENTS ]; /* client socket fd list */
  int client_socket_index = 0;  /* next free spot */
  /* ====== */


  /* Create the listener socket as TCP socket */
  /*   (use SOCK_DGRAM for UDP)               */
  int sock = socket( PF_INET, SOCK_STREAM, 0 );
    /* note that PF_INET is protocol family, Internet */

  if ( sock < 0 )
  {
    perror( "socket()" );
    exit( EXIT_FAILURE );
  }

  /* socket structures from /usr/include/sys/socket.h */
  struct sockaddr_in server;
  struct sockaddr_in client;

  server.sin_family = PF_INET;
  server.sin_addr.s_addr = INADDR_ANY;

  unsigned short port = 8127;

  /* htons() is host-to-network-short for marshalling */
  /* Internet is "big endian"; Intel is "little endian" */
  server.sin_port = htons( port );
  int len = sizeof( server );

  if ( bind( sock, (struct sockaddr *)&server, len ) < 0 )
  {
    perror( "bind()" );
    exit( EXIT_FAILURE );
  }

  listen( sock, 5 );  /* 5 is number of waiting clients */
  printf( "Listener bound to port %d\n", port );

  int fromlen = sizeof( client );

  char buffer[ BUFFER_SIZE ];

  int i, n;

  while ( 1 )
  {

#if 0
    struct timeval timeout;
    timeout.tv_sec = 3;
    timeout.tv_usec = 500;  /* 3 AND 500 microseconds */
#endif

    FD_ZERO( &readfds );
    FD_SET( sock, &readfds );
    printf( "Set FD_SET to include listener fd %d\n", sock );

    for ( i = 0 ; i < client_socket_index ; i++ )
    {
      FD_SET( client_sockets[ i ], &readfds );
      printf( "Set FD_SET to include client socket fd %d\n",
              client_sockets[ i ] );
    }

#if 1
    /* BLOCK */
    /*this select call checks all of the sockets connections, if it gets a 
    message on one, it prints out the one that does and loops through all the
    connections checking for and printing messages*/
    int ready = select( FD_SETSIZE, &readfds, NULL, NULL, NULL );
    /* ready is the number of ready file descriptors */
    printf( "select() identified %d descriptor(s) with activity\n", ready );
#endif

#if 0
    int ready = select( FD_SETSIZE, &readfds, NULL, NULL, &timeout );
    if ( ready == 0 ) {
      printf( "No activity\n" );
      continue;
    }
#endif

    if ( FD_ISSET( sock, &readfds ) )
    {
      int newsock = accept( sock,
                            (struct sockaddr *)&client,
                            (socklen_t *)&fromlen );
             /* this accept() call we know will not block */
      printf( "Accepted client connection\n" );
      client_sockets[ client_socket_index++ ] = newsock;
    }

    for ( i = 0 ; i < client_socket_index ; i++ )
    {
      int fd = client_sockets[ i ];

      if ( FD_ISSET( fd, &readfds ) )
      {
        /* can also use read() and write() */
        n = recv( fd, buffer, BUFFER_SIZE - 1, 0 );

        if ( n < 0 )
        {
          perror( "recv()" );
        }
        else if ( n == 0 )
        {
          int k;
          printf( "Client on fd %d closed connection\n", fd );
          close( fd );
          /* remove fd from client_sockets[] array: */
          for ( k = 0 ; k < client_socket_index ; k++ )
          {
            if ( fd == client_sockets[ k ] )
            {
              /* found it -- copy remaining elements over fd */
              int m;
              for ( m = k ; m < client_socket_index - 1 ; m++ )
              {
                client_sockets[ m ] = client_sockets[ m + 1 ];
              }
              client_socket_index--;
              break;  /* all done */
            }
          }
        }
        else
        {
          buffer[n] = '\0';
          printf( "Received message from %s: %s\n",
                  inet_ntoa( (struct in_addr)client.sin_addr ),
                  buffer );

          /* send ack message back to client */
          n = send( fd, "ACK\n", 4, 0 );
          if ( n != 4 )
          {
            perror( "send() failed" );
          }
        }
      }
    }
  }

  return EXIT_SUCCESS; /* we never get here */
}
