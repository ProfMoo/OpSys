/* main.c */

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
#define TEST 567343

int main()
{
  int x = XYZ;   /* x is statically allocated memory
                    (4 bytes are allocated on the runtime stack) */
  long y = TEST;

  printf( "sizeof int is %d bytes\n", (int)sizeof( int ) );
  printf( "sizeof long is %d bytes\n", (int)sizeof(y) );

  printf( "x is %d\n", x );

#ifdef DEBUG_MODE
  printf( "here1\n" );
  printf( "here3\n" );
  printf( "here12\n" );
  printf( "here18\n" );
  printf( "here21\n" );
#endif

  return EXIT_SUCCESS;
}
