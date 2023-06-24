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
	char* test_name = "Joe";
	char* _raColTEST_my_name = STRING(test_name);
	int _raColTEST_test_status = 0;
	int _raColTEST_pipefd[2];
	pipe(_raColTEST_pipefd);
	if (!fork()) {
		dup2(_raColTEST_pipefd[1], STDOUT_FILENO);
		close(_raColTEST_pipefd[0]);
		close(_raColTEST_pipefd[1]);
		int conditional = 1;
		char* details = "Main test function";
		if (!conditional) {
			printf(STRING(conditional) " %s", details);
			return -1;
		}
		else {
			printf("%s passed\n", test_name);
		}
		return 0;
	}
	else {
		close(_raColTEST_pipefd[1]);
		int _raColTEST_pid_status;
		if (wait(&_raColTEST_pid_status)) {
			_raColTEST_test_status = 0;
			printPipe(_raColTEST_pipefd);
			/* print_stacktrace;
			   print captured_stdout;
			log all data; \ */
		}
		else {
			_raColTEST_test_status = 1;
		}
	}
}
