#ifndef raColTest_macros_h
#define raColTest_macros_h

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <semaphore>

#include "sys_utils.h"
#include "logger.h"

#define STRING(string) #string

#define TEST(_test_name) \
{ \
	const char* raColTest_test_name = _test_name; \
	try { \

#define ASSERT(conditional, details) \
		if (!(conditional)) { \
			printf("\"%s\" failed:\n\t", raColTest_test_name); \
			std::string msg = STRING(conditional); \
			msg += "\t"; \
			msg += details; \
			logger::log(logger::FAIL, argv[0], raColTest_test_name, msg.c_str()); \
			printf(STRING(conditional) "\n\t%s\n", details); \
		} \
		else { \
			printf("\"%s\" passed\n", raColTest_test_name); \
			logger::log(logger::PASS, argv[0], raColTest_test_name, "\0"); \
		} \

#define END_TEST \
	} \
	catch(std::exception& e) { \
		printf("\"%s\" encountered an exception:\n\t%s\n", raColTest_test_name, e.what()); \
		logger::log(logger::ERROR, argv[0], raColTest_test_name, e.what()); \
		logger::log(logger::STD_OUT, argv[0], raColTest_test_name, "stdout placeholder"); \
		/* _raColTest_test_status = 1; */\
	} \
}

#endif
