#ifndef sys_utils_h
#define sys_utils_h

namespace rCT_sys {
	// status: standard return from syscalls
	// msg: full msg to display in case of fail status
	char* good_strcat(const char* str1, const char* str2);
	int error_handler(int status, const char* msg);
	int test_handler(int status, const char* func_name);
	FILE* fopen_handler(FILE* status, const char* file_name);
	int fclose_handler(int status, const char* file_name);
	int fflush_handler(int status, const char* file_name);
	int fork_handler();
	int pipe_handler(int* pipefd);
	int close_handler(int fd, const char* file_name);
	void print_pipe(int* pipefd);
	// file_name is automatically concat to msg
	int io_handler(int status, const char* file_name, const char* msg);
}

#endif
