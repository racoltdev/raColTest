#include "input_parser.h"

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


std::vector<char*> rCT_input::input_parser(int argc, char** argv) {
	std::vector<char*> suites;
	for (int i = 0; i < argc; i++) {
		char* arg = argv[i];
		if (strcmp(arg, "-s") == 0 || strcmp(arg, "--suite") == 0) {
			i++;
			if (i >= argc) {
				fprintf(stderr, "raColTest: Error! --suite argument must be followed by a path to the suite\n");
				exit(1);
			}
			suites.push_back(argv[i]);
		}
	}
	return suites;
}
