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
	const char* raColTest_test_name = _test_name; \
	fflush(stdout); \
	int raColTest_saved_stdout = dup(1); \
	int raColTest_std_cap = open("captured_stdout", O_TMPFILE|O_RDWR|O_EXCL, S_IRUSR|S_IWUSR); \
	dup2(raColTest_std_cap, 1); \
	close(raColTest_std_cap); \
 \
	logger::data_type raColTest_status = logger::ERROR; \
	try { \

#define ASSERT(raColTest_conditional, raColTest_details) \
		if (!(raColTest_conditional)) { \
			std::string raColTest_msg = STRING(raColTest_conditional); \
			raColTest_msg += "\t"; \
			raColTest_msg += raColTest_details; \
			raColTest_status = logger::FAIL; \
			logger::log(raColTest_status, argv[0], raColTest_test_name, raColTest_msg.c_str()); \
			fflush(stdout); \
			logger::log_captured_stdout(argv[0], raColTest_test_name, stdout); \
		} \
		else { \
			raColTest_status = logger::PASS; \
			logger::log(raColTest_status, argv[0], raColTest_test_name, "\0"); \
		} \
		fflush(stdout); \
		dup2(raColTest_saved_stdout, 1); \
		close(raColTest_saved_stdout); \

#define END_TEST \
	} \
	catch(std::exception& e) { \
		logger::log(raColTest_status, argv[0], raColTest_test_name, e.what()); \
		fflush(stdout); \
		logger::log_captured_stdout(argv[0], raColTest_test_name, stdout); \
		fflush(stdout); \
		dup2(raColTest_saved_stdout, 1); \
		close(raColTest_saved_stdout); \
	} \
}

#endif
