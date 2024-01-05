#ifndef raColTest_macros_h
#define raColTest_macros_h

#include <stdio.h>
#include <sys/wait.h>
#include <exception>
#include <cstdlib>
// TODO if i ever get multithreading running, this needs semaphores to interact with logger
//#include <semaphore>

#include "sys_utils.h"
#include "logger.h"
#include "ANSI-color-codes.h"

#define TEST(_test_name) \
{ \
	/* Make sure there's nothing in stdout */ \
	rCT_sys::fflush_handler( \
		fflush(stdout), \
		"stdout" \
	); \
	const char* raColTest_test_name = _test_name; \
	int raColTest_pipefd[2]; \
	rCT_sys::test_handler( \
		pipe(raColTest_pipefd), \
		"pipe()" \
	); \
	/* saved_stdout points to same place as stdout fileno */ \
	int raColTest_saved_stdout = rCT_sys::test_handler( \
		dup(STDOUT_FILENO), \
		"dup(STDOUT_FILENO)" \
	); \
	/* STDOUT_FILENO now refers to the same fp as raColTest_pipefd[1] */ \
	/* This affects STDOUT_FILENO and stdout */ \
	rCT_sys::test_handler( \
		dup2(raColTest_pipefd[1], STDOUT_FILENO), \
		"dup2(write_pipe, STDOU_TFILENO" \
	); \
	/* I can immediately close raColTest_pipefd[1] since stdout fileno points to the right place */ \
	rCT_sys::test_handler( \
		close(raColTest_pipefd[1]), \
		"close(write_pipe)" \
	); \
	logger::data_type raColTest_status = logger::ERROR; \
	try { \

#define ASSERT(raColTest_conditional, raColTest_details) \
		/* Data may have been written but not made it through the buffer yet. Clear it */ \
		if (fflush(stdout) == EOF) { \
			perror("Fatal error (fflush(stdout)). raColTest is exiting this test!"); \
			exit(-1); \
		} \
		/* raColTest_pipefd[0] waits for it's write end to close to get EOF. stdout points to the same place as it's write end, so redirecting it acts like an EOF */ \
		rCT_sys::test_handler( \
			dup2(raColTest_saved_stdout, STDOUT_FILENO), \
			"dup2(saved_stdout, STDOUT_FILENO)" \
		); \
		if (!(raColTest_conditional)) { \
			const char* raColTest_msg1 = "assert(" #raColTest_conditional "): ";\
			const char* raColTest_msg = rCT_sys::good_strcat(raColTest_msg1, raColTest_details); \
			raColTest_status = logger::FAIL; \
			logger::log(raColTest_status, argv[0], raColTest_test_name, raColTest_msg); \
			/* send logger the read end of the pipe */ \
			logger::log_captured_stdout(argv[0], raColTest_test_name, raColTest_pipefd[0]); \
		} \
		else { \
			raColTest_status = logger::PASS; \
			logger::log(raColTest_status, argv[0], raColTest_test_name, "\0"); \
		}

#define END_TEST \
	} \
	catch(std::exception& e) { \
		/* Data may have been written but not made it through the buffer yet. Clear it */ \
		rCT_sys::fflush_handler( \
			fflush(stdout), \
			"stdout" \
		); \
		/* raColTest_pipefd[0] waits for it's write end to close to get EOF. stdout points to the same place as it's write end, so redirecting it acts like an EOF */ \
		rCT_sys::test_handler( \
			dup2(raColTest_saved_stdout, STDOUT_FILENO), \
			"dup2(saved_stdout, STDOUT_FILENO)" \
		); \
		logger::log(raColTest_status, argv[0], raColTest_test_name, e.what()); \
		/* send logger the read end of the pipe */ \
		logger::log_captured_stdout(argv[0], raColTest_test_name, raColTest_pipefd[0]); \
	} \
	/* read end is no longer needed */ \
	/* This should have error checking, but it borks for some reason */ \
	close(raColTest_pipefd[0]); \
	if (raColTest_status == logger::PASS) { \
		printf(GRNB "p" RESET); \
	} else if (raColTest_status == logger::FAIL) { \
		printf(REDB "F" RESET);\
	} \
	else if (raColTest_status == logger::ERROR) { \
		printf(REDB "e" RESET); \
	} \
}

#endif
