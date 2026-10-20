#ifndef raColTest_macros_h
#define raColTest_macros_h

#include <cstdlib>
// TODO if i ever get multithreading running, this needs semaphores to interact with logger
//#include <semaphore>

#include "sys_utils.h"
#include "test_builder.h"
#include "logger.h"

#define TEST(_test_name) \
{ \
	int raColTest_pipefd[2]; \
	rCT_test::TestContext raColTest_context = rCT_test::open_test_pipe(raColTest_pipefd); \
	raColTest_context.test_name = (char*) _test_name; \
	raColTest_context.status = logger::ERROR; \
	raColTest_context.test_file = (char*) __FILE__; \
	try { /* Require semicolon after macro call */ \
		static_assert(true, "")

#define ASSERT(raColTest_conditional, raColTest_details) \
	/* Require semicolon after macro call */ \
	do { \
		const char* msg1 = "assert(" #raColTest_conditional "): "; \
		char* msg_d = rCT_sys::good_strcat(msg1, raColTest_details); \
		rCT_test::build_assert(raColTest_conditional, msg_d, &raColTest_context); \
		free(msg_d); \
	} while (false)

#define END_TEST \
	} \
	catch(std::exception& e) { \
		rCT_test::end_and_catch(e, raColTest_context); \
	} \
	rCT_test::end_and_close(raColTest_context); \
} /* Require semicolon after macro call */ \
static_assert(true, "")

#endif
