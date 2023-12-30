#include "../src/raColTest_macros.h"

#include <stdexcept>
#include <fcntl.h>

int main(int , char** argv) {
	TEST("Passing test")
		printf("I'm in a passing test!\n");
		ASSERT(1 == 1, "This should pass")
	END_TEST

	TEST("Failing test")
		printf("I'm in a failing test\n");
		ASSERT(1 == 0, "Another test should pass after this")
	END_TEST

	TEST("Continues after fail")
		ASSERT(1 == 1, "This will never fail")
	END_TEST

	TEST("Errors correctly")
		printf("I'm in an except test\n");
		throw std::runtime_error("Intentional test throw");
		ASSERT(1 == 1, "This will never fail")
	END_TEST

	TEST("Continues after error")
		ASSERT(1 == 1, "This will never fail")
	END_TEST

	//TEST("Second include")
	//getFuncs("raColTest");
	//const char* output = "true";
	//	ASSERT(output != NULL, "Linking failed")
	//END_TEST
}
