/* buffering-v2.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {

	int* xyz = NULL;

	printf( "HERE1\n" ); //adding this newline ensures that you will see HERE1 printed before it faults.
						 //this is good for debugging

	fflush(NULL); //look in the big comment below for explanation. basically, \n on steroids

	*xyz = 123; //this is bad, so it causes a seg fault

	printf( "HERE2\n" ); 

	/* if you do /a.out > output.txt, HERE1 doesnt show up in terminal or 
	in output.txt! make sure that the buffer is flushed before a problem. In this case
	the \n isnt an automatic flush of the buffer */

	return EXIT_SUCCESS;
}