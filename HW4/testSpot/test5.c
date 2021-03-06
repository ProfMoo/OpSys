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
  int n;
  char* msg;
  char buffer[BUFFER_SIZE];
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

  struct hostent * hp = gethostbyname( "OpSys" );


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

  //========================================================================
  //      COMMAND ONE
  //========================================================================
  //sending command
 // msg = "PUT mouse.txt 917\nAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBBAAAAABBBBB7777777";
  

  //msg = "PUT mouse.txt 917\nOnce when a Lion was asleep a little Mouse began running up and down upon him; this soon wakened the Lion, who placed his huge paw upon him, and opened his big jaws to swallow him. \"Pardon, O King,\" cried the little Mouse: \"forgive me this time, I shall never forget it: who knows but what I may be able to do you a turn some of these days?\" The Lion was so tickled at the idea of the Mouse being able to help him, that he lifted up his paw and let him go. Some time after the Lion was caught in a trap, and the hunters, who desired to carry him alive to the King, tied him to a tree while they went in search of a wagon to carry him on. Just then the little Mouse happened to pass by, and seeing the sad plight in which the Lion was, sent up to him and soon gnawed away the ropes that bound the King of the Beasts. \"Was I not right?\" said the little Mouse.\n\"LITTLE FRIENDS MAY PROVE GREAT FRIENDS.\"";

  msg = "PUT mouse.txt 917\nOnce when a Lion was asleep a little Mouse began running up and down upon him; this soon wakened the Lion, who placed his huge paw upon him, and opened his big jaws to swallow him. \"Pardon, O King,\" cried the little Mouse: \"forgive me this time, I shall never forget it: who knows but what I may be able to do you a turn some of these days?\" The Lion was so tickled at the idea of the Mouse being able to help him, that he lifted up his paw and let him go. Some time after the Lion was caught in a trap, and the hunters, who desired to carry him alive to the King, tied him to a tree while they went in search of a wagon to carry him on. Just then the little Mouse happened to pass by, and seeing the sad plight in which the Lion was, sent up to him and soon gnawed away the ropes that bound the King of the Beasts. \"Was I not right?\" said the little Mouse.                \"LITTLE FRIENDS MAY PROVE GREAT FRIENDS.\"";

  n = write( sd, msg, strlen( msg ) );

  if ( n < strlen( msg ) )
  {
    perror( "write() failed" );
    return EXIT_FAILURE;
  }

 
  //getting ACK back
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

  //========================================================================
  //      COMMAND TWO
  //========================================================================
  //sending command
  msg = "GET xyz.jpg 5555 2000\n";
  n = write( sd, msg, strlen( msg ) );

  if ( n < strlen( msg ) )
  {
    perror( "write() failed" );
    return EXIT_FAILURE;
  }

 
  //getting ACK back
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

    //========================================================================
  //      COMMAND THREE
  //========================================================================
  //sending command
  msg = "LIST\n";
  n = write( sd, msg, strlen( msg ) );

  if ( n < strlen( msg ) )
  {
    perror( "write() failed" );
    return EXIT_FAILURE;
  }

 
  //getting ACK back
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

  //////////////////////////////////////////////////////////////////
  //PART TWOOWPDJEWginaroiuhnbaoeiuntosiernatboirnsugqterbrientabiurnsfbvaoeir
  ///////////////////////////////////////////////////////////////////

  /* create TCP client socket (endpoint) */
  sd = socket( PF_INET, SOCK_STREAM, 0 );

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

  hp = gethostbyname( "OpSys" );


  if ( hp == NULL )
  {
    fprintf( stderr, "ERROR: gethostbyname() failed\n" );
    return EXIT_FAILURE;
  }

  server.sin_family = AF_INET;
  memcpy( (void *)&server.sin_addr, (void *)hp->h_addr, hp->h_length );
  port = 8123;
  server.sin_port = htons( port );

  printf( "server address is %s\n", inet_ntoa( server.sin_addr ) );


  printf( "connecting to server.....\n" );
  if ( connect( sd, (struct sockaddr *)&server, sizeof( server ) ) == -1 )
  {
    perror( "connect() failed" );
    return EXIT_FAILURE;
  }

  //========================================================================
  //      COMMAND ONE
  //========================================================================
  //sending command
  msg = "PUT mouse.txt 14\nHELLOHELLOHELL";
  n = write( sd, msg, strlen( msg ) );

  if ( n < strlen( msg ) )
  {
    perror( "write() failed" );
    return EXIT_FAILURE;
  }

 
  //getting ACK back
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

  //========================================================================
  //      COMMAND TWO
  //========================================================================
  //sending command
  msg = "PUT chicken.txt 31\nHELLOTHEREHELLOTHEREHELLOTHEREH";
  n = write( sd, msg, strlen( msg ) );

  if ( n < strlen( msg ) )
  {
    perror( "write() failed" );
    return EXIT_FAILURE;
  }

 
  //getting ACK back
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

    //========================================================================
  //      COMMAND THREE
  //========================================================================
  //sending command
  msg = "GET chicken.txt 29 1";
  n = write( sd, msg, strlen( msg ) );

  if ( n < strlen( msg ) )
  {
    perror( "write() failed" );
    return EXIT_FAILURE;
  }

 
  //getting ACK back
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

      //========================================================================
  //      COMMAND FOUR
  //========================================================================
  //sending command
  msg = "GET chicken.txt 30 2";
  n = write( sd, msg, strlen( msg ) );

  if ( n < strlen( msg ) )
  {
    perror( "write() failed" );
    return EXIT_FAILURE;
  }

 
  //getting ACK back
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


  return EXIT_SUCCESS;
}
