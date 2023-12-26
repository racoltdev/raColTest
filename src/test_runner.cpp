#include <unistd.h>
#include <filesystem>
#include <vector>
#include <iostream>
#include <string.h>
#include <sys/wait.h>

#include "sys_utils.h"

std::vector<std::string> collect_tests() {
	std::vector<std::string> test_names;
	try {
		for (const auto& entry : std::filesystem::directory_iterator("test")) {
			const std::string p = std::filesystem::absolute(entry.path());
			test_names.push_back(p);
			printf("\tCollected %s\n", p.c_str());
		}
		if (test_names.empty()) {
			printf("\tNo tests were collected!");
		}
		return test_names;
	}
	catch (std::filesystem::filesystem_error& e) {
		puts(e.what());
		puts("raColTest encountered an error. You must have a test directory");
		exit(-1);
	}
}

char* isolate_fname(char* path) {
	size_t fname_start = 0;
	size_t path_len = strlen(path);
	// Always safe to start search at [-2]
	for (int i = path_len - 2; i >= 0; i--) {
		char delim = '/';
		if (path[i] == delim) {
			fname_start = i + 1;
			break;
		}
	}
	size_t fname_size = path_len - fname_start;
	char* fname = (char*) malloc(sizeof(char) * fname_size);
	strcpy(fname, path + fname_start);
	return fname;
}

int exec_file(char* path) {
	printf("\tExecuting %s.........", path);
	pid_t pid;
	int pipefd[2];
	rCT_sys::pipe_handler(pipefd);
	pid = rCT_sys::fork_handler();

	if (pid == 0) {
		rCT_sys::error_handler(\
			dup2(pipefd[1], STDOUT_FILENO), \
			"Cannot dup pipefd to stdout" \
		);
		rCT_sys::close_handler(pipefd[0], "pipefd");
		rCT_sys::close_handler(pipefd[1], "pipefd");
		// Normally I should free this, but this execution env gets wiped in the next line anyways
		char* fname = isolate_fname(path);
		rCT_sys::error_handler( \
			execl(path, fname, NULL), \
			"FAIL\n\t\tFailed to execute test file" \
		);
	} else {
		printf("\n");
		int status;
		rCT_sys::error_handler( \
			waitpid(pid, &status, 0), \
			"\t\tError while waiting for test to finish" \
		);
		rCT_sys::close_handler(pipefd[1], "pipefd");
		if (WIFSIGNALED(status)) {
			printf("\t\tTest was stopped by signal: %d\n", WTERMSIG(status));
		} else {
			rCT_sys::print_pipe(pipefd);
		}
		rCT_sys::close_handler(pipefd[0], "pipefd");
	}
	return 0;
}

int exec_test(std::string& path) {
	const std::string test = "test";
	std::string::size_type index = path.find(test);
	path.replace(index, test.length(), "testbin");
	int length = path.length();
	char* test_path = path.data();
	// Remove '.cpp'
	test_path[length - 4] = '\0';
	exec_file(test_path);
	return 0;
}

int test_runner() {
	printf("Collecting tests.......\n");
	std::vector<std::string> tests = collect_tests();
	printf("Executing tests........\n");
	for (std::string& test : tests) {
		exec_test(test);
	}
	return 0;
}
