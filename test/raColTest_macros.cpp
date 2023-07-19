#include "../src/raColTest_macros.h"
#include "../src/symbol_parser.h"

int main() {
	TEST("Passing test")
	ASSERT(1 == 1, "This should pass")
	END_TEST

	TEST("Failing test")
	ASSERT(1 == 0, "This should fail")
	END_TEST

	TEST("Print test")
	ASSERT(true, "This should pass")
	END_TEST

	TEST("Print test2")
	ASSERT(true, "This should pass")
	END_TEST

	//TEST("Second include")
	//getFuncs("raColTest");
	//const char* output = "true";
	//ASSERT(output != NULL, "Linking failed")
	//END_TEST
}
