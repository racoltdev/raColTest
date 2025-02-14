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

void get_config_file(const char** config_path) {
	if (CONFIG_SOURCE == UNDEF) {
		CONFIG_SOURCE = find_config();
	}

	switch(CONFIG_SOURCE) {
		case DEFAULT:
			*config_path = DEFAULT_CONFIG_PATH;
			char err_msg[256];
			sprintf(err_msg, "\n\tConfig file not found in expected location: %s\n\tMake sure raColTest has been installed with `make install`.", *config_path);
			if (access(*config_path, F_OK) != 0) {
				throw std::runtime_error(std::string(err_msg));
			}
			break;
		case LOCAL:
			*config_path = LOCAL_CONFIG_PATH;
			break;
		case GLOBAL:
			*config_path = GLOBAL_CONFIG_PATH;
			break;
		default:
			break;
	}
}

void parse_string(const char* key, const char** value) {
	const char* config_file;
	get_config_file(&config_file);
	scl::config_file rfile(config_file, scl::config_file::READ);
	*value = strdup(rfile.get<std::string>(key).c_str());
	rfile.close();
}

void parse_int(const char* key, int* value) {
	const char* config_file;
	get_config_file(&config_file);
	scl::config_file rfile(config_file, scl::config_file::READ);
	*value = rfile.get<int>(key);
	rfile.close();
}

void parse_bool(const char* key, bool* value) {
	const char* config_file;
	get_config_file(&config_file);
	scl::config_file rfile(config_file, scl::config_file::READ);
	*value = rfile.get<bool>(key);
	rfile.close();
}

const char* config::logfile_name() {
	static const char* logfile_name = "";
	if (logfile_name[0] == '\0') {
		parse_string("logfile_name", &logfile_name);
	}
	return logfile_name;
}

int config::timeout() {
	static int timeout;
	static bool timeout_set = false;
	if (!timeout_set) {
		parse_int("timeout", &timeout);
		timeout_set = true;
	}
	return timeout;
}

const char* config::test_source_dir() {
	static const char* test_source_dir = "";
	if (test_source_dir[0] == '\0') {
		parse_string("test_source_dir", &test_source_dir);
	}
	return test_source_dir;
}

const char* config::test_bin_dir() {
	static const char* test_bin_dir = "";
	if (test_bin_dir[0] == '\0') {
		parse_string("test_bin_dir", &test_bin_dir);
	}
	return test_bin_dir;
}

bool config::enable_github_status() {
	static bool enable_github_status;
	static bool enable_github_status_set = false;
	if (!enable_github_status_set) {
		parse_bool("enable_github_status", &enable_github_status);
	}
	return enable_github_status;
}

int config::verbosity() {
	static int verbosity;
	static bool verbosity_set = false;
	if (!verbosity_set) {
		parse_int("verbosity", &verbosity);
	}
	return verbosity;
}
