#include <unistd.h>
#include <filesystem>
#include <vector>
#include <iostream>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#include "lib_raColTest/sys_utils.h"
#include "lib_raColTest/logger.h"
#include "lib_raColTest/ANSI-color-codes.h"

std::vector<std::string> collect_tests() {
	std::vector<std::string> test_names;
	try {
		for (const auto& entry : std::filesystem::directory_iterator("test")) {
			const std::string p = std::filesystem::absolute(entry.path());
			test_names.push_back(p);
			printf("\tCollected %s\n", p.c_str());
		}
		if (test_names.empty()) {
			printf("\tNo tests were collected!\n");
		}
		return test_names;
	}
	catch (std::filesystem::filesystem_error& e) {
		puts(e.what());
		puts("raColTest encountered an error. You must have a test directory");
		exit(2);
	}
}

char* isolate_fname(char* path, int parents) {
	int children = 0;
	size_t fname_start = 0;
	size_t path_len = strlen(path);
	char delim = '/';
	// Always safe to start search at [-2]
	for (int i = path_len - 2; i >= 0; i--) {
		if (path[i] == delim) {
			fname_start = i + 1;
			if (++children == parents) {
				break;
			}
		}
	}
	size_t fname_size = path_len - fname_start;
	char* fname = (char*) malloc(sizeof(char) * fname_size);
	strcpy(fname, path + fname_start);
	return fname;
}

void exec_file(char* path) {
	// Normally I should free this, but this execution env gets wiped in the next line anyways
	char* fname = isolate_fname(path, 2);
	fflush(stdout);
	printf("\tExecuting %s.........", fname);
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
		int status = execl(path, fname, NULL);
		if (status > 0) {
			perror("FAIL\n\t\tFailed to execute test file" );
			logger::log(logger::ERROR, fname, fname, "Failed to execute test file");
		}
	} else {
		printf("\n");
		int status;
		char error_blip[16] = "";
		rCT_sys::error_handler( \
			waitpid(pid, &status, 0), \
			"\t\tFatal error while waiting for test to finish, Quiting raColTest" \
		);
		rCT_sys::close_handler(pipefd[1], "pipefd");
		if (WIFSIGNALED(status)) {
			char msg[64];
			sprintf(msg, "Test was killed by signal: %d (%s)", WTERMSIG(status), strsignal(WTERMSIG (status)));
			logger::log(logger::ERROR, fname, fname, msg);
			strcpy(error_blip, YELB "E" RESET ".....");
		}
		else if (WIFSTOPPED(status)) {
			char msg[64];
			sprintf(msg, "Test was stopped by signal: %d (%s)", WSTOPSIG(status), strsignal(WSTOPSIG (status)));
			logger::log(logger::ERROR, fname, fname, msg);
			strcpy(error_blip, YELB "E" RESET ".....");
		}
		printf("\t\t");
		rCT_sys::print_pipe(pipefd);
		printf("%s", error_blip);
		printf("\n");
		rCT_sys::close_handler(pipefd[0], "pipefd");
	}
}

void exec_test(std::string& path) {
	const std::string test = "/test/";
	std::string::size_type index = path.find(test);
	path.replace(index, test.length(), "/testbin/");
	int length = path.length();
	char* test_path = path.data();
	// Remove '.cpp'
	test_path[length - 4] = '\0';
	exec_file(test_path);
}

void test_runner() {
	time_t start = time(NULL);
	printf("Collecting tests.......\n");
	std::vector<std::string> tests = collect_tests();
	printf("Executing tests........\n");
	for (std::string& test : tests) {
		exec_test(test);
	}
	time_t end = time(NULL);
	printf("\nFinished executing tests\n\n");
	logger::display(start, end);
}
