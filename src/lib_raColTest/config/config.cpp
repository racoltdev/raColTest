#include "config.h"

#include <cstddef>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <filesystem>

#include "scl/SCL.hpp"

#define DEFAULT_CONFIG_PATH "/etc/raColTest/defaults.config"
#define LOCAL_CONFIG_PATH "raColTest.config"
#define GLOBAL_CONFIG_PATH "~/.config/raColTest/raColTest.config"

enum config_source {UNDEF, DEFAULT, LOCAL, GLOBAL};
static config_source CONFIG_SOURCE = UNDEF;

config_source find_config() {
	if (access(LOCAL_CONFIG_PATH, F_OK) == 0) {
		return LOCAL;
	} else if (access(GLOBAL_CONFIG_PATH, F_OK) == 0) {
		return GLOBAL;
	} else {
		return DEFAULT;
	}
}

void parse_config(const char* key, const char** value) {
	if (CONFIG_SOURCE == UNDEF) {
		CONFIG_SOURCE = find_config();
	}

	const char* config_path;
	switch(CONFIG_SOURCE) {
		case DEFAULT:
			config_path = DEFAULT_CONFIG_PATH;
			char err_msg[256];
			sprintf(err_msg, "\n\tConfig file not found in expected location: %s\n\tMake sure raColTest has been installed with `make install`.", config_path);
			if (access(config_path, F_OK) != 0) {
				throw std::runtime_error(std::string(err_msg));
			}
			break;
		case LOCAL:
			config_path = LOCAL_CONFIG_PATH;
			break;
		case GLOBAL:
			config_path = GLOBAL_CONFIG_PATH;
			break;
		default:
			break;
	}
	printf("%s\n", config_path);
	scl::config_file rfile(config_path, scl::config_file::READ);
	const char* val = rfile.get<std::string>(key).c_str();
	*value = strdup(val);
}

const char* config::logfile_name() {
	static const char* logfile_name = "";
	if (logfile_name[0] == '\0') {
		parse_config("logfile_name", &logfile_name);
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
