#include "lib_raColTest/macros.h"

int main(int, char** argv) {
	TEST("Executes tests correctly")
		ASSERT(true, "Failed to execute test")
	END_TEST

	TEST("Prints sig kills")
		raise(SIGSEGV);
		ASSERT(false, "This should exit and print")
	END_TEST
}
