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

#define ASSERT(conditional, details) \
	if (!(conditional)) { \
		printf("%s failed:\n\t", raColTest_test_name); \
		printf(STRING(conditional) "\n\t%s\n", details); \
	} \
	else { \
		printf("%s passed\n", raColTest_test_name); \
	} \

#define END_TEST \
	_raColTest_test_status = 1; \
}
