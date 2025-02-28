#include <vector>
#include <stdio.h>

#include "test_runner.h"
#include "input_parser.h"

int main(int argc, char** argv) {
	std::vector<char*> suites = rCT_input::input_parser(argc - 1, argv + 1);
	test_runner(suites);
}
