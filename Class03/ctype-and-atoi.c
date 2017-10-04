#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main()
{
	char buffer[100];
	sprintf( buffer, "%d)(xewo'?.iwefj", 1234);
	printf( "buffer is: %s\n", buffer);

	int n = atoi(buffer); /* also atof() */

	printf( "Converted \"%s\" to %d\n", buffer, n);

	if (isdigit( buffer[1] ))
	{
		printf( "'%c' is a digit\n", buffer[1] );
	}
	if (isalpha( buffer[7] )) 
	{
		printf( "'%c' is an alpha character\n", buffer[7]);
	}
}

//check out "strings-and-printf.c' from online. helpful for strcpy and sprintf functions