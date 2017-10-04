#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
	printf( "argc is %d\n", argc);

	printf("argv[0] is %s\n", argv[0]);
	printf("argv[1] is %s\n", argv[1]);
	printf("argv[2] is %s\n", argv[2]);
	printf("argv[3] is %s\n", argv[3]);

	//these can be manipulated just like any array ^^^
	char* tmp = argv[0];
	argv[0] = argv[1];
	argv[1] = tmp;


#if 0
	base$ ./a.out abc 123 xyz

	char* argv[];

	argv
	+--+
[0]	|  | --> ./a.out
	|  |
	+--+
[1]	|  | --> abc
	|  |
	+--+
[2]	|  | --> 123
	|  |
	+--+
[3]	|  | --> xyz
	|  |
	+--+
#endif

}