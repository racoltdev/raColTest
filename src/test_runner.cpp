#include "test_runner.h"

#include <unistd.h>
#include <filesystem>
#include <vector>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/stat.h>

#include "lib_raColTest/sys_utils.h"
#include "lib_raColTest/logger.h"
#include "lib_raColTest/ANSI-color-codes.h"
#include "lib_raColTest/config/config.h"
#include "lib_raColTest/files.h"

std::vector<std::string> collect_tests(const char* suite) {
	try {
		std::vector<std::string> test_names;
		const char* root_dir;
		if (suite == NULL) {
			root_dir = config::test_source_dir();
		} else {
			root_dir = suite;
		}
		printf("%s\n", root_dir);
		// lambda function
		auto printer = [](std::string p) {
			printf("\tCollected %s\n", p.c_str());
		};
		test_names = rCT_files::find(root_dir, printer);

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

void exec_file(char* path) {
	char* fname_d = rCT_files::truncate_path_d(path, getenv("PWD"));
	fflush(stdout);
	printf("\tExecuting %s.........", fname_d);
	pid_t pid;
	int pipefd[2];
	rCT_sys::pipe_handler(pipefd);
	pid = rCT_sys::fork_handler();

	if (pid == 0) {
		rCT_sys::error_handler(
			dup2(pipefd[1], STDOUT_FILENO),
			"Cannot dup pipefd to stdout"
		);
		// Not needed for child
		rCT_sys::close_handler(pipefd[0], "pipefd");
		// Can be closed since stdout sends into the pipe now
		rCT_sys::close_handler(pipefd[1], "pipefd");

		alarm(config::timeout());
		int status = execl(path, fname_d, NULL);
		if (status > 0) {
			perror("FAIL\n\t\tFailed to execute test file" );
			logger::log(logger::ERROR, fname_d, fname_d, "Failed to execute test file");
		}
	} else {
		printf("\n");
		int status;
		char error_blip[16] = "";

		rCT_sys::error_handler( \
			waitpid(pid, &status, 0), \
			"\t\tFatal error while waiting for test to finish, Quiting raColTest" \
		);
		// Not needed for parent
		rCT_sys::close_handler(pipefd[1], "pipefd");

		if (WIFSIGNALED(status)) {
			char msg[64];
			sprintf(msg, "Test was killed by signal: %d (%s)", WTERMSIG(status), strsignal(WTERMSIG (status)));
			logger::log(logger::ERROR, fname_d, fname_d, msg);
			// logger::log(logger::STD_OUT, fname_d, fname_d, "");
			strcpy(error_blip, YELB "E" RESET ".....");
		}
		else if (WIFSTOPPED(status)) {
			char msg[64];
			sprintf(msg, "Test was stopped by signal: %d (%s)", WSTOPSIG(status), strsignal(WSTOPSIG (status)));
			logger::log(logger::ERROR, fname_d, fname_d, msg);
			// TODO figure out if I should capture stdout from sig kills
			// logger::log(logger::STD_OUT, fname_d, fname_d, "");
			strcpy(error_blip, YELB "E" RESET ".....");
		}
		printf("\t\t");
		rCT_sys::print_pipe(pipefd);
		printf("%s", error_blip);
		printf("\n");
		rCT_sys::close_handler(pipefd[0], "pipefd");
	}
	free(fname_d);
}

void exec_test(std::string& path) {
	std::string test = "/";
	test.append(config::test_source_dir());
	size_t index = path.find(test);

	std::string testbin = "/";
	testbin.append(config::test_bin_dir());
	path.replace(index, test.length(), testbin);

	size_t length = path.length();
	char* test_path = path.data();
	// Remove '.cpp'
	test_path[length - 4] = '\0';
	exec_file(test_path);
}

void set_status_check(bool pass) {
	const char* status_dir = ".github/";
	mkdir(status_dir, 0700);
	const char* status_path = ".github/status";

	FILE* status_file = rCT_sys::fopen_handler( \
		fopen(status_path, "w"), \
		status_path \
	);

	if (pass) {
		fprintf(status_file, "%s\n", "pass");
	} else {
		fprintf(status_file, "%s\n", "fail");
	}

	rCT_sys::fclose_handler( \
		fclose(status_file), \
		status_path \
	);
}

void produce_status(bool pass) {
	if (config::enable_github_status()) {
		set_status_check(pass);
	}

	if (pass) {
		printf(GRNB "All tests passed successfully.\nBuild successful!" RESET "\n");
	} else {
		printf(REDB "Some tests failed!" RESET "\n");
	}
}

void test_runner(std::vector<char*> suites) {
	time_t start = time(NULL);
	printf("Collecting tests.......\n");
	std::vector<std::string> tests;
	for (char* suite : suites) {
		std::vector<std::string> suite_tests = collect_tests(suite);
		tests.insert(tests.end(), suite_tests.begin(), suite_tests.end());
	}
	if (suites.size() == 0) {
		tests = collect_tests(NULL);
	}
	printf("Executing tests........\n");
	for (std::string& test : tests) {
		exec_test(test);
	}
	time_t end = time(NULL);
	printf("\nFinished executing tests\n\n");

	bool pass = logger::display(start, end);
	produce_status(pass);
}
