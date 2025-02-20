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
	int raColTest_pipefd[2]; \
	rCT_test::TestContext raColTest_context = rCT_test::open_test_pipe(raColTest_pipefd); \
	raColTest_context.test_name = _test_name; \
	raColTest_context.status = logger::ERROR; \
	raColTest_context.test_file = argv[0]; \
	try { \

#define ASSERT(raColTest_conditional, raColTest_details) \
	/* This bracket can cause -Wmisleading-indentation */ \
	{ \
		const char* msg1 = "assert(" #raColTest_conditional "):"; \
		const char* msg = rCT_sys::good_strcat(msg1, raColTest_details); \
		rCT_test::build_assert(raColTest_conditional, msg, &raColTest_context); \
	} \

#define END_TEST \
	} \
	catch(std::exception& e) { \
		rCT_test::end_and_catch(e, raColTest_context); \
	} \
	rCT_test::end_and_close(raColTest_context); \
}

#endif
