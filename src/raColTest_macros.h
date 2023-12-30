#ifndef raColTest_macros_h
#define raColTest_macros_h

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string>
#include <fcntl.h>
// TODO if i ever get multithreading running, this needs semaphores to interact with logger
//#include <semaphore>

#include "sys_utils.h"
#include "logger.h"

#define STRING(string) #string

#define TEST(_test_name) \
{ \
	fflush(stdout); \
	const char* raColTest_test_name = _test_name; \
	int raColTest_saved_stdout = dup(STDOUT_FILENO); \
	int pipefd[2]; \
	pipe(pipefd); \
	dup2(pipefd[1], STDOUT_FILENO); \
	logger::data_type raColTest_status = logger::ERROR; \
	try { \

#define ASSERT(raColTest_conditional, raColTest_details) \
		if (!(raColTest_conditional)) { \
			std::string raColTest_msg = STRING(raColTest_conditional); \
			raColTest_msg += "\t"; \
			raColTest_msg += raColTest_details; \
			raColTest_status = logger::FAIL; \
			close(pipefd[1]); \
			logger::log(raColTest_status, argv[0], raColTest_test_name, raColTest_msg.c_str()); \
			logger::log_captured_stdout(argv[0], raColTest_test_name, pipefd[0]); \
		} \
		else { \
			raColTest_status = logger::PASS; \
			logger::log(raColTest_status, argv[0], raColTest_test_name, "\0"); \
		} \
		fflush(stdout); \
		close(pipefd[0]); \
		dup2(raColTest_saved_stdout, STDOUT_FILENO); \

#define END_TEST \
	} \
	catch(std::exception& e) { \
		logger::log(raColTest_status, argv[0], raColTest_test_name, e.what()); \
		logger::log_captured_stdout(argv[0], raColTest_test_name, pipefd[0]); \
		fflush(stdout); \
		close(pipefd[0]); \
		dup2(raColTest_saved_stdout, STDOUT_FILENO); \
	} \
}

#endif
