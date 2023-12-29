#include "raColTest_macros.h"

#include "logger.h"

int main(int, char** argv) {
	TEST("Logger imported correctly")
		ASSERT(true, "Dummy test")
	END_TEST

	return 0;
}
