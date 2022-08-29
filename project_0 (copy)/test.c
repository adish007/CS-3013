#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main () {
	char *buffer;
	strncpy(buffer, "Hello, world!", 14);
	printf("%s\n", buffer);
	exit(0);
}

