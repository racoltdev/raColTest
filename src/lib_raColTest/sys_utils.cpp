#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cerrno>

#define BUFF_SIZE 128

namespace rCT_sys {
	char* good_strcat(const char* str1, const char* str2) {
		char* newstr = (char*)malloc(strlen(str1) + strlen(str2) + 1);
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
		int err = errno;
		if (status < 0) {
			perror(msg);
			exit(err);
		}
		return status;
	}

	int test_handler(int status, const char* func_name) {
		int err = errno;
		if (status < 0) {
			const char* msg1 = "Fatal error (";
			char* msg2 = good_strcat(msg1, func_name);
			const char* msg3 = "). raColTest is exiting this test!";
			char* msg = good_strcat(msg2, msg3);
			perror(msg);
			exit(err);
			free(msg2);
			free(msg);
		}
		return status;
	}

	FILE* fopen_handler(FILE* status, const char* file_name) {
		int err = errno;
		if (status == NULL) {
			const char* msg = good_strcat("Failed to open ", file_name);
			perror(msg);
			exit(err);
		}

		return status;
	}

	int fclose_handler(int status, const char* file_name) {
		if (status == EOF) {
			const char* msg = good_strcat("[Non fatal] failed to close ", file_name);
			perror(msg);
		}
		return status;
	}

	int fflush_handler(int status, const char* file_name) {
		int err = errno;
		if (status == EOF) {
			const char* msg1 = "Fatal error (fflush(";
			const char* msg2 = good_strcat(msg1, file_name);
			const char* msg3 = "stdout)). raColTest is exiting this test!";
			const char* msg = good_strcat(msg2, msg3);
			perror(msg);
			exit(err);
		}
		return status;
	}

	int io_handler(int status, const char* file_name, const char* msg) {
		if (status < 0) {
			char* description = good_strcat(msg, file_name);
			error_handler(status, description);
			free(description);
		}
		return status;
	}

	int fork_handler() {
		return error_handler(fork(), "Failed to fork");
	}

	int pipe_handler(int* pipefd) {
		return error_handler(pipe(pipefd), "Opening pipe failed");
	}

	int close_handler(int fd, const char* file_name) {
		const char* msg = "Failed to close fd ";
		return io_handler(close(fd), file_name, msg);
	}
}
