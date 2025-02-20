#ifndef raColTest_test_builder_h
#define raColTest_test_builder_h

#include <exception>

#include "logger.h"

int open_test_pipe(int* pipefd);
logger::data_type build_assert(bool pass, const char* msg, int* pipefd, const char* test_name, int saved_stdout, const char* test_file);
void end_and_catch(int saved_stdout, logger::data_type status, const char* test_file, const char* test_name, int* pipefd, std::exception& e);
void end_and_close(int* pipefd, logger::data_type status);
#endif
