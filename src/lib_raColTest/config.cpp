#include "config.h"

#include <cstddef>

const char* config::logfile_name() {
	static const char* logfile_name = "";
	if (logfile_name[0] == '\0') {
		logfile_name = "project.log";
	}
	return logfile_name;
}

int config::timeout() {
	static int timeout;
	static bool timeout_set = false;
	if (!timeout_set) {
		timeout = 2;
		timeout_set = true;
	}
	return timeout;
}

const char* config::test_source_dir() {
	static const char* test_source_dir = "";
	if (test_source_dir[0] == '\0') {
		test_source_dir = "test";
	}
	return test_source_dir;
}

const char* config::test_bin_dir() {
	static const char* test_bin_dir = "";
	if (test_bin_dir[0] == '\0') {
		test_bin_dir = "testbin";
	}
	return test_bin_dir;
}

bool config::enable_github_status() {
	static bool enable_github_status;
	static bool enable_github_status_set = false;
	if (!enable_github_status_set) {
		enable_github_status = false;
	}
	return enable_github_status;
}

int config::verbosity() {
	static int verbosity;
	static bool verbosity_set = false;
	if (!verbosity_set) {
		verbosity = 1;
	}
	return verbosity;
}
