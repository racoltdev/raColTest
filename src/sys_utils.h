#ifndef sys_utils_h
#define sys_utils_h
#endif

namespace rCT_sys {
	int error_handler(int status, const char* msg);
	int fork_handler();
	int pipe_handler(int* pipefd);
	int close_handler(int fd, const char* file_name);
	void print_pipe(int* pipefd);
	int io_handler(int status, const char* file_name, const char* msg);
}