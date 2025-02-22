#ifndef LOGGER_H
#define LOGGER_H

#include <time.h>
#include <stdio.h>

namespace logger {
	// ERROR is meant to catch stderr messages and any error handling code
	// FAIL encodes a failed test
	// PASS encodes a passing test
	// STANDARD_OUT is any other data that would normally be printed during execution
	// if data_type < 2 then test failed
	enum data_type : char {ERROR, FAIL, PASS, STD_OUT};

	void log(data_type msg_type, char* test_file, char* test_name, const char* data);
	void log_captured_stdout(char* test_file, char* test_name, int std_cap_fd);
	bool display(time_t start_time, time_t end_time);
};

#endif
