// raColTest main
#include <stdio.h>
#include <execinfo.h>

#define assert(x) ((x) ? return 0 : errorHandler(#x, x))

float public = 1.3;

void aFunc(int hello) {
	printf("%d", hello);
}

int main() {
	int joe = 12;
	char* string = "mama";
	aFunc(joe);
	return 0;
}
