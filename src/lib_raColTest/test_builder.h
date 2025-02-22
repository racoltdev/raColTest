#ifndef raColTest_test_builder_h
#define raColTest_test_builder_h

#include <exception>

#include "logger.h"

namespace rCT_test {
	struct TestContext {
		char* test_name;
		char* test_file;
		int* pipefd;
		int saved_stdout;
		logger::data_type status;
	};

	TestContext open_test_pipe(int* pipefd);
	void build_assert(bool pass, const char* msg, TestContext* context);
	void end_and_catch(std::exception& e, TestContext context);
	void end_and_close(TestContext context);
}

#endif
