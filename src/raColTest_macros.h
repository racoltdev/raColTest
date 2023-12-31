#ifndef raColTest_macros_h
#define raColTest_macros_h

#include <stdio.h>
#include <sys/wait.h>
#include <string>
// TODO if i ever get multithreading running, this needs semaphores to interact with raColTest_logger
//#include <semaphore>

#include "sys_utils.h"
#include "logger.h"

#define TEST(_test_name) \
{ \
	/* Make sure there's nothing in stdout */ \
	fflush(stdout); \
	const char* raColTest_test_name = _test_name; \
	int pipefd[2]; \
	pipe(pipefd); \
	int write_pipe = pipefd[1]; \
	int read_pipe = pipefd[0]; \
	/* saved_stdout points to same place as stdout fileno */ \
	int raColTest_saved_stdout = dup(STDOUT_FILENO); \
	/* STDOUT_FILENO now refers to the same fp as write_pipe */ \
	/* This affects STDOUT_FILENO and stdout */ \
	dup2(write_pipe, STDOUT_FILENO); \
	/* I can immediately close write_pipe since stdout fileno points to the right place */ \
	close(write_pipe); \
	raColTest_logger::data_type raColTest_status = raColTest_logger::ERROR; \
	try { \

#define ASSERT(raColTest_conditional, raColTest_details) \
		/* Data may have been written but not made it through the buffer yet. Clear it */ \
		fflush(stdout); \
		/* read_pipe waits for it's write end to close to get EOF. stdout points to the same place as it's write end, so redirecting it acts like an EOF */ \
		dup2(raColTest_saved_stdout, STDOUT_FILENO); \
		if (!(raColTest_conditional)) { \
			std::string raColTest_msg = "assert(" #raColTest_conditional "): ";\
			raColTest_msg += raColTest_details; \
			raColTest_status = raColTest_logger::FAIL; \
			raColTest_logger::log(raColTest_status, argv[0], raColTest_test_name, raColTest_msg.c_str()); \
			/* send logger the read end of the pipe */ \
			raColTest_logger::log_captured_stdout(argv[0], raColTest_test_name, read_pipe); \
		} \
		else { \
			raColTest_status = raColTest_logger::PASS; \
			raColTest_logger::log(raColTest_status, argv[0], raColTest_test_name, "\0"); \
		}

#define END_TEST \
	} \
	catch(std::exception& e) { \
		/* Data may have been written but not made it through the buffer yet. Clear it */ \
		fflush(stdout); \
		/* read_pipe waits for it's write end to close to get EOF. stdout points to the same place as it's write end, so redirecting it acts like an EOF */ \
		dup2(raColTest_saved_stdout, STDOUT_FILENO); \
		raColTest_logger::log(raColTest_status, argv[0], raColTest_test_name, e.what()); \
		/* send logger the read end of the pipe */ \
		raColTest_logger::log_captured_stdout(argv[0], raColTest_test_name, read_pipe); \
	} \
	/* read end is no longer needed */ \
	close(read_pipe); \
	/* Just use string literals for term colors to avoid cluttering end user namespace */ \
	if (raColTest_status == raColTest_logger::PASS) { \
		printf("\e[42m" "p" "\e[0m"); \
	} else if (raColTest_status == raColTest_logger::FAIL) { \
		printf("\e[41m" "F" "\e[0m"); \
	} \
	else if (raColTest_status == raColTest_logger::ERROR) { \
		printf("\e[41m" "e" "\e[0m"); \
	} \
}

#endif
