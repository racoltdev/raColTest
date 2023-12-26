#ifndef LOGGER_H
#define LOGGER_H

namespace logger {
	// ERROR is meant to catch stderr messages and any error handling code
	// FAIL encodes a failed test
	// PASS encodes a passing test
	// STANDARD_OUT is any other data that would normally be printed during execution
	// if data_type < 2 then test failed
	enum data_type {ERROR, FAIL, PASS, STD_OUT};

	void log(data_type msg_type, char* test_file, const char* test_name, const char* data);
}

#endif
