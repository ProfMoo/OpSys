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

/**
  PROCESS (running program residing in memory):

  +-------------------+
  | runtime heap      | <== dynamically allocated memory
  |  |                |   -- the "new" keyword in C++/Java
  |  v                |   -- in C, it's malloc(), calloc(), realloc()
  |                   |   -- dont forget to use free() ... or we have a memory leak :(
  |                   |
  |                   |
  |                   |
  |                   |
  |  ^   qrs()        |
  |  |   xyz()        |
  |  |   main()       |
  | runtime stack     | <== statically allocated memory 
  |-------------------|  -- (eg. int x, int* y, char c, etc)
  | code/data/etc     |
  +-------------------+
**/

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
  //casting void* as int*

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

  //now using more types of memory allocation
  int* z = NULL;

  z = calloc( 50, sizeof( int) ); /* dynamically allocate 200 bytes. because 50 int (int are byte size 4) is 200 */
  /*this is the same as: malloc( 50 * sizeof( int ) ), but calloc() zeroes the memory. this is safer
  calloc() makes array dynamically */

  if (z == NULL) {
    fprintf( stderr, "ERORR: calloc() failed\n" );
    return EXIT_FAILURE;
  }

  z[10] = 123;
  printf("z[10] is %d\n", z[10]);
  *(z+10) = 456; /* z + 10 * sizeof(int)  */
  printf("z[10] is %d\n", z[10]);

  //z[200] = 555; //this will change the value, but wont give any compile or run errors

  z = realloc( z, 100*sizeof(int) ); /*we use the same 200 bytes, and add 200 onto it.
                                      it might move the dynamic memory so that it can all be in a row. 
                                      this could be the same as free if you put in size of 0*/

  z[80] = 678;
  printf("z[80] is %d\n", z[80]);

  free(z);

  return EXIT_SUCCESS;
}



