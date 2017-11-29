/* strings-and-printf.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char * argv[] )
{
  char name[80];
  sprintf( name, "David %s", argv[1] );

  char name2[80];
  strcpy( name2, name );
  strcpy( name2, "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );

  printf( "name is: %s\n", name );
  printf( "name2 is: %s\n", name2 );

  int len = strlen( name2 );
  printf( "length of name2 is %04d\n", len );


  char name3[80];
  sprintf( name3, "QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ" );

  /* copy the first 10 chars/bytes from name2 into name3 */
  strncpy( name3, name2, 10 );
  printf( "name3 is %s\n", name3 );

    /* "ABCDEFGHIJ"
       "ABCDEFGHIJQQQQQQQQQQQQQQ..." */


  /* example of string concatenation */
  char path[100];
  int q = sprintf( path, "/cs/goldsd/" );
  strcpy( path + q, "ABCDEFGH.txt" );
  printf( "path is %s\n", path );

  path[10] = '\0';
  printf( "path is %s\n", path );

  /* TO DO: check out man pages for strlen(), strcpy(), strncpy(),
            sprintf(), strcmp(), strncmp(), strstr(), strchr(), strcat(),
            strtok(), etc. */

  return EXIT_SUCCESS;
}
