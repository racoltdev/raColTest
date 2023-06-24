#ifndef test_macros_h
#define test_macros_h
#endif

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

#define STRING(string) #string

#define TEST(_test_name) \
{ \
	char* raColTEST_test_name = _test_name; \
	int _raColTEST_test_status = 0; \
	int _raColTEST_pipefd[2]; \
	pipe(_raColTEST_pipefd); \
	if (!fork()) { \
		dup2(_raColTEST_pipefd[1], STDOUT_FILENO);

#define ASSERT(conditional, details) \
	close(_raColTEST_pipefd[0]); \
	close(_raColTEST_pipefd[1]); \
	if (!conditional) { \
		printf("%s failed:\n\t", raColTEST_test_name); \
		printf(STRING(conditional) "\n\t%s\n", details); \
		return -1; \
	} \
	else { \
		printf("%s passed\n", raColTEST_test_name); \
	} \

#define END_TEST \
	return 0;} \
	else { \
		close(_raColTEST_pipefd[1]); \
		int _raColTEST_pid_status; \
		if (wait(&_raColTEST_pid_status)) { \
			_raColTEST_test_status = 0; \
			printPipe(_raColTEST_pipefd); \
			/* print_stacktrace; \ print captured_stdout; \ log all data; \ */ } \
			else { \
				_raColTEST_test_status = 1; \
			} \
	} \
}

void printPipe(int* pipefd);
