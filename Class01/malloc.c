/* malloc.c */

/* compile this code as follows (from bash shell or terminal):

   bash$ gcc -Wall -Werror main.c

   once compiled, execute this code as follows:

   bash$ ./a.out
*/

/* to compile in "debug mode," do the following:

   bash$ gcc -Wall -Werror -D DEBUG_MODE main.c
*/

/* anything that starts with '#' is a preprocessor directive ... */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define XYZ 42

int main()
{
  int x = XYZ;   /* x is statically allocated memory
                    (4 bytes are allocated on the runtime stack) */

  printf( "sizeof int is %d bytes\n", (int)sizeof( int ) );

  printf( "x is %d\n", x );


  int * y = NULL;  /* y is statically allocated memory
                      (? bytes are allocated on the runtime stack) */

  printf( "sizeof int * is %d bytes\n", (int)sizeof( int * ) );

  y = malloc( sizeof( int ) );   /* y points to 4 bytes of dynamically
                                     allocated memory on the runtime heap */

  /*  y = (int *)malloc( sizeof( int ) );  */

  if ( y == NULL )   /* always check return value from system calls! */
  {
    fprintf( stderr, "ERROR: malloc() failed\n" );
    return EXIT_FAILURE;
  }

  *y = 456;

  printf( "*y is %d\n", *y );

  free( y );
  y = NULL;


#if 0
  printf( "*y is %d\n", *y );
#endif

  return EXIT_SUCCESS;
}



