#include "lib_raColTest/macros.h"

#include "lib_raColTest/logger.h"

int main() {
	TEST("Logger imported correctly")
		ASSERT(true, "Dummy test")
	END_TEST

	return 0;
}
