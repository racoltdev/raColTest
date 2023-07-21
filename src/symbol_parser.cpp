// No tests, but as of e30f8a2, this seems correct
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFF_SIZE 128

int sysErrorHandler(int status, const char* msg) {
	if (status == -1) {
		perror(msg);
		exit(-1);
	}
	return status;
}

int closePipe(int fd) {
	return sysErrorHandler(close(fd), "Failed to close pipe");
}

char** getFuncs(const char* file_path) {
	pid_t pid;
	int pipefd[2];
	int nestedPipefd[2];
	FILE *cmd_output;
	char buff[BUFF_SIZE];

	// 0 is read end
	// 1 is write end
	sysErrorHandler(pipe(pipefd), "Failed to create pipe");
	sysErrorHandler(pipe(nestedPipefd), "Failed to create pipe");

	pid = sysErrorHandler(fork(), "Fork failed");
	// Use nestedPipefd for child process interactions
	// Use pipefd for child/parent interactions
	if (pid == 0) {
		sysErrorHandler(\
				dup2(nestedPipefd[1], STDOUT_FILENO),\
				"Cannot dup pipefd to stdout");
		closePipe(pipefd[1]);
		closePipe(pipefd[0]);
		closePipe(nestedPipefd[0]);
		closePipe(nestedPipefd[1]);

		sysErrorHandler(
				execlp("nm", "nm", "-f", "sysv", file_path, (char*) NULL),\
				"Failed to execute nm\n");
	}
	else {
		pid = sysErrorHandler(fork(), "Fork failed");
		if (pid == 0) {
			sysErrorHandler(\
					dup2(nestedPipefd[0], STDIN_FILENO),\
					"Cannot dup pipefd to stdin");
			sysErrorHandler(\
					dup2(pipefd[1], STDOUT_FILENO),\
					"Cannot dup pipefd to stdout");
			closePipe(pipefd[0]);
			closePipe(pipefd[1]);
			closePipe(nestedPipefd[0]);
			closePipe(nestedPipefd[1]);

			sysErrorHandler(\
				execlp("awk", "awk", "-F|", "$1 !~ /^_start\\s*/ && $3 ~ /T/ && $7 ~ /.text/ {print}", (char*) NULL),\
				"Failed to execute awk");
		}
		else {
			int status;
			closePipe(pipefd[1]);
			closePipe(nestedPipefd[0]);
			closePipe(nestedPipefd[1]);
			sysErrorHandler(\
					waitpid(pid, &status, 0),\
					"Error while waiting for child");
		}
	}

	// you have to use fdopen() and fgets() here
	// read() can't handle streams/pipes
	cmd_output = fdopen(pipefd[0], "r");
	while(fgets(buff, sizeof buff, cmd_output)) {
		printf("%s", buff);
	}
	fclose(cmd_output);
	return 0;
}
