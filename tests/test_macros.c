#include "../src/raColTest_macros.h"

int main() {
	TEST("Passing test")
	ASSERT(1 == 1, "This should pass")
	END_TEST

	TEST("Failing test")
	ASSERT(1 == 0, "This should fail")
	END_TEST
}
