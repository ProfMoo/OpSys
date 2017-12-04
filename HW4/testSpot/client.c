//342 bytes in def.txt

#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <strings.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main()
{
  /* create TCP client socket (endpoint) */
  int sd = socket( PF_INET, SOCK_STREAM, 0 );

  if ( sd < 0 )
  {
    perror( "socket() failed" );
    exit( EXIT_FAILURE );
  }

#if 0
  /* localhost maps to 127.0.0.1, which stays on the local machine */
  struct hostent * hp = gethostbyname( "localhost" );

  struct hostent * hp = gethostbyname( "128.113.126.29" );
#endif

  struct hostent * hp = gethostbyname( "TheOffice" );


  if ( hp == NULL )
  {
    fprintf( stderr, "ERROR: gethostbyname() failed\n" );
    return EXIT_FAILURE;
  }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  memcpy( (void *)&server.sin_addr, (void *)hp->h_addr, hp->h_length );
  unsigned short port = 8123;
  server.sin_port = htons( port );

  printf( "server address is %s\n", inet_ntoa( server.sin_addr ) );


  printf( "connecting to server.....\n" );
  if ( connect( sd, (struct sockaddr *)&server, sizeof( server ) ) == -1 )
  {
    perror( "connect() failed" );
    return EXIT_FAILURE;
  }


  /* The implementation of the application layer protocol is below... */

  char * msg = "GET abc.txt 40 50";
  int n = write( sd, msg, strlen( msg ) );

  if ( n < strlen( msg ) )
  {
    perror( "write() failed" );
    return EXIT_FAILURE;
  }

  char buffer[ BUFFER_SIZE ];
  n = read( sd, buffer, BUFFER_SIZE -1 );    /* BLOCKING */

  if ( n == -1 )
  {
    perror( "read() failed" );
    return EXIT_FAILURE;
  }
  else if ( n == 0 )
  {
    printf( "Rcvd no data; also, server socket was closed\n" );
  }
  else
  {
    buffer[n] = '\0';    /* assume we rcvd text-based data */
    printf( "Rcvd from server: %s\n", buffer );
  }

    n = read( sd, buffer, BUFFER_SIZE -1 );    /* BLOCKING */

  if ( n == -1 )
  {
    perror( "read() failed" );
    return EXIT_FAILURE;
  }
  else if ( n == 0 )
  {
    printf( "Rcvd no data; also, server socket was closed\n" );
  }
  else
  {
    buffer[n] = '\0';    /* assume we rcvd text-based data */
    printf( "Rcvd from server: %s\n", buffer );
  }

  close( sd );

  return EXIT_SUCCESS;
}
