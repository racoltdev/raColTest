#ifndef raColTest_macros_h
#define raColTest_macros_h

#include <stdio.h>
#include <sys/wait.h>
#include <exception>
// TODO if i ever get multithreading running, this needs semaphores to interact with logger
//#include <semaphore>

#include "sys_utils.h"
#include "logger.h"
#include "ANSI-color-codes.h"
#include "test_builder.h"

#define TEST(_test_name) \
{ \
	const char* raColTest_test_name = _test_name; \
	int raColTest_pipefd[2]; \
	int raColTest_saved_stdout = open_test_pipe(raColTest_pipefd); \
	logger::data_type raColTest_status = logger::ERROR; \
	try { \

#define ASSERT(raColTest_conditional, raColTest_details) \
	/* TODO use a struct that contains all this info so I dont have to open a new scope here */ \
	{ \
		const char* msg1 = "assert(" #raColTest_conditional "):"; \
		const char* msg = rCT_sys::good_strcat(msg1, raColTest_details); \
		raColTest_status = build_assert( \
				raColTest_conditional, \
				msg, \
				raColTest_pipefd, \
				raColTest_test_name, \
				raColTest_saved_stdout, \
				argv[0] \
			  ); \
	} \

#define END_TEST \
	} \
	catch(std::exception& e) { \
		end_and_catch( \
				raColTest_saved_stdout, \
				raColTest_status, \
				argv[0], \
				raColTest_test_name, \
				raColTest_pipefd, \
				e \
			); \
	} \
	end_and_close(raColTest_pipefd, raColTest_status); \
}

#endif
