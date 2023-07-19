#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define BUFF_SIZE 128

namespace rCT_sys {
	char* good_strcat(const char* str1, const char* str2) {
		char* newstr = (char*)malloc(strlen(str1) + strlen(str2) + 1); // might not need the +1?
		strcpy(newstr, str1);
		strcat(newstr, str2);
		return newstr;
	}

	void print_pipe(int* pipefd) {
		char buff[BUFF_SIZE] = {0};
		FILE* cmd_output = fdopen(pipefd[0], "r");
		while(fgets(buff, sizeof buff, cmd_output)) {
			printf("%s", buff);
		}
	}

	int error_handler(int status, const char* msg) {
		if (status == -1) {
			perror(msg);
			exit(-1);
		}
		return status;
	}

	int io_handler(int status, const char* file_name, const char* msg) {
		char* description = good_strcat(msg, file_name);
		return error_handler(status, description);
	}

	int fork_handler() {
		return error_handler(fork(), "Failed to fork");
	}

	int pipe_handler(int* pipefd) {
		return error_handler(pipe(pipefd), "Opening pipe failed");
	}

	int close_handler(int fd, const char* file_name) {
		const char* msg = "Failed to close fd ";
		io_handler(close(fd), file_name, msg);
	}
}