#include "config.h"

#include <cstddef>

char* config::logfile_name() {
	return "project.log";
}

int config::timeout() {
	return 1;
}

char* config::test_source_dir() {
	static char* test_source_dir = "";
	if (test_source_dir[0] == '\0') {
		test_source_dir = "test";
	}
	return test_source_dir;
}

char* config::test_bin_dir() {
	return "testbin";
}

bool config::enable_github_status() {
	return false;
}

int config::verbosity() {
	return 1;
}
