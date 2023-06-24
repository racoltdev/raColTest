#include "test_macros.h"

#define BUFF_SIZE 128

void printPipe(int* pipefd) {
	char buff[BUFF_SIZE];
	FILE* cmd_output = fdopen(pipefd[0], "r");
		while(fgets(buff, sizeof buff, cmd_output)) {
			printf("%s", buff);
		}
	fclose(cmd_output);
}

int main() {
	TEST("First Test")
	ASSERT(1 == 1, "This should pass")
	END_TEST
}
