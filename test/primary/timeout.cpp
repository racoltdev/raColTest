#include "lib_raColTest/macros.h"

int main(int, char** argv) {
	TEST("Timeout kills")
	while(true) {}
	ASSERT(1==1, "This will never pass")
	END_TEST
}
