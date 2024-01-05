#include "raColTest/macros.h"

#include "raColTest/logger.h"

int main(int, char** argv) {
	TEST("Logger imported correctly")
		ASSERT(true, "Dummy test")
	END_TEST

	return 0;
}
