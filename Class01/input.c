#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

int main() {

	char strarray[5][8];
	char str[8];
	char temp[8];
	int j;
	int i;
	int count = 5;

	i = 0;
	while (i < count) {
		printf("Enter a name: ");
		scanf("%s", str);
		strcpy(strarray[i], str);
		//printf("i: %d\n", i);
		//printf("Entered: %s\n", strarray[i]);
		i++;
	}
	printf("\n");

	i = 0;
	while (i < count) {
		//printf("i: %d\n", i);
		printf("You entered: %s\n", strarray[i]);
		i++;
	}
	printf("\n");

	printf("Going to sort now...\n\n");
	for (i = 0; i < count; i++) {
		for (j = i+1; j < count; j++) {
			if(strcmp(strarray[i], strarray[j])>0) {
				strcpy(temp,strarray[i]);
				strcpy(strarray[i], strarray[j]);
				strcpy(strarray[j], temp);
			}
		}
	}

	i = 0;
	printf("Sorted: \n");
	while (i < count) {
		printf("i: %d\n", i);
		printf("%s\n", strarray[i]);
		i++;
	}

	return EXIT_SUCCESS;
}