#ifndef raColTest_macros_h
#define raColTest_macros_h
#endif

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

#include "sys_utils.h"

#define STRING(string) #string

#define TEST(_test_name) \
{ \
	const char* raColTest_test_name = _test_name; \
	int _raColTest_test_status = 0; \
	int _raColTest_pipefd[2]; \
	rCT_sys::pipe_handler(_raColTest_pipefd); \
	if (!rCT_sys::fork_handler()) { \
		rCT_sys::error_handler( \
			dup2(_raColTest_pipefd[1], STDOUT_FILENO), \
			"dup2 failed"\
		);

#define ASSERT(conditional, details) \
	rCT_sys::close_handler(_raColTest_pipefd[0], "Child, pipefd[0]"); \
	rCT_sys::close_handler(_raColTest_pipefd[1], "Child, pipefd[1]"); \
	if (!(conditional)) { \
		printf("%s failed:\n\t", raColTest_test_name); \
		printf(STRING(conditional) "\n\t%s\n", details); \
		return -1; \
	} \
	else { \
		printf("%s passed\n", raColTest_test_name); \
	} \

#define END_TEST \
	return 0;} \
	else { \
		rCT_sys::close_handler(_raColTest_pipefd[1], "Parent, pipefd[1]"); \
		int _raColTest_pid_status; \
		int waitStatus = rCT_sys::error_handler( \
			wait(&_raColTest_pid_status), \
			"Wait failed" \
		); \
		if (waitStatus) { \
			_raColTest_test_status = 0; \
			rCT_sys::print_pipe(_raColTest_pipefd); \
			rCT_sys::close_handler(_raColTest_pipefd[0], "Parent, pipefd[0]"); \
			/* print_stacktrace; \ print captured_stdout; \ log all data; \ */ } \
			else { \
				_raColTest_test_status = 1; \
			} \
	} \
}
