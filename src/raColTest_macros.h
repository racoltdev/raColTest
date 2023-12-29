#ifndef raColTest_macros_h
#define raColTest_macros_h

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string>
// TODO if i ever get multithreading running, this needs semaphores to interact with logger
//#include <semaphore>

#include "sys_utils.h"
#include "logger.h"

#define STRING(string) #string

#define TEST(_test_name) \
{ \
	const char* raColTest_test_name = _test_name; \
	try { \

#define ASSERT(raColTest_conditional, raColTest_details) \
		if (!(raColTest_conditional)) { \
			printf("\t\t\"%s\" failed:\n", raColTest_test_name); \
			std::string raColTest_msg = STRING(raColTest_conditional); \
			raColTest_msg += "\t"; \
			raColTest_msg += raColTest_details; \
			logger::log(logger::FAIL, argv[0], raColTest_test_name, raColTest_msg.c_str()); \
			printf("\t\t\t" STRING(raColTest_conditional) "\n\t\t\t%s\n", raColTest_details); \
		} \
		else { \
			printf("\t\t\"%s\" passed\n", raColTest_test_name); \
			logger::log(logger::PASS, argv[0], raColTest_test_name, "\0"); \
		} \

#define END_TEST \
	} \
	catch(std::exception& e) { \
		printf("\t\t\"%s\" encountered an exception:\n\t\t\t%s\n", raColTest_test_name, e.what()); \
		logger::log(logger::ERROR, argv[0], raColTest_test_name, e.what()); \
		logger::log(logger::STD_OUT, argv[0], raColTest_test_name, "stdout placeholder"); \
	} \
}

#endif
