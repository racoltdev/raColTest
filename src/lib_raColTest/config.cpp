#include "config.h"

char* config::logfile_name() {
	return "project.log";
}
int config::timeout() {
	return 1;
}
char* config::test_source_dir() {
	return "test";
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

int getConfigData(config::Config c) {
	c.logfile_name = config::logfile_name();
	c.test_source_dir = config::test_source_dir();
	c.test_bin_dir = config::test_bin_dir();
	c.verbosity = config::verbosity();
	c.timeout = config::timeout();
	c.enable_github_status = config::enable_github_status();
	return 0;
}
