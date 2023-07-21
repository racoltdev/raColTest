#include <unistd.h>
#include <filesystem>
#include <vector>
#include <iostream>
#include <string.h>
#include <sys/wait.h>

#include "sys_utils.h"

std::vector<std::string> collect_tests() {
	std::vector<std::string> test_names;
	for (const auto& entry : std::filesystem::directory_iterator("test")) {
		const std::string p = std::filesystem::absolute(entry.path());
		test_names.push_back(p);
		printf("\tCollected %s\n", p.c_str());
	}
	return test_names;
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
			"[raColTest] Cannot dup pipefd to stdout" \
		);
		rCT_sys::close_handler(pipefd[0], "pipefd");
		rCT_sys::close_handler(pipefd[1], "pipefd");
		rCT_sys::error_handler( \
			execl(path, path, NULL), \
			"FAIL\n\t\tFailed to execute test file" \
		);
	} else {
		printf("\n");
		int status;
		rCT_sys::error_handler( \
			waitpid(pid, &status, 0), \
			"[raColTest] Error while waiting for test to finish" \
		);
		rCT_sys::close_handler(pipefd[1], "pipefd");
		// TODO pipe doesn't clear between reads, resulting in echoing of all previous prints
		//		the buffer is cleared correctly
		rCT_sys::print_pipe(pipefd);
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
