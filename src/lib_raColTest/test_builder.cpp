#include "test_builder.h"

#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <string>
#include <exception>

#include "sys_utils.h"
#include "logger.h"
#include "ANSI-color-codes.h"

int open_test_pipe(int* pipefd) {
	/* Make sure there's nothing in stdout */
	rCT_sys::fflush_handler(
		fflush(stdout),
		"stdout"
	);
	rCT_sys::test_handler(
		pipe(pipefd),
		"pipe()"
	);
	/* saved_stdout points to same place as stdout fileno */
	int raColTest_saved_stdout = rCT_sys::test_handler(
		dup(STDOUT_FILENO),
		"dup(STDOUT_FILENO)"
	);
	/* STDOUT_FILENO now refers to the same fp as raColTest_pipefd[1] */
	/* This affects STDOUT_FILENO and stdout */
	rCT_sys::test_handler(
		dup2(pipefd[1], STDOUT_FILENO),
		"dup2(write_pipe, STDOUT_FILENO)"
	);
	/* I can immediately close raColTest_pipefd[1] since stdout fileno points to the right place */
	rCT_sys::test_handler(
		close(pipefd[1]),
		"close(write_pipe)"
	);

	return raColTest_saved_stdout;
}

logger::data_type build_assert(bool pass, const char* msg, int* pipefd, const char* test_name, int saved_stdout, const char* test_file) {
		/* Data may have been written but not made it through the buffer yet. Clear it */
		if (fflush(stdout) == EOF) {
			int err = errno;
			perror("Fatal error (fflush(stdout)). raColTest is exiting this test!");
			exit(err);
		}

		/* raColTest_pipefd[0] waits for it's write end to close to get EOF. stdout points to the same place as it's write end, so redirecting it acts like an EOF */
		rCT_sys::test_handler(
			dup2(saved_stdout, STDOUT_FILENO),
			"dup2(saved_stdout, STDOUT_FILENO)"
		);

		logger::data_type status;
		if (!pass) {
			status = logger::FAIL;
			logger::log(status, test_file, test_name, msg);
			/* send logger the read end of the pipe */
			logger::log_captured_stdout(test_file, test_name, pipefd[0]);
		}
		else {
			status = logger::PASS;
			logger::log(status, test_file, test_name, "\0");
		}
		return status;
}

void end_and_catch(int saved_stdout, logger::data_type status, const char* test_file, const char* test_name, int* pipefd, std::exception& e) {
		/* Data may have been written but not made it through the buffer yet. Clear it */
		rCT_sys::fflush_handler(
			fflush(stdout),
			"stdout"
		);
		/* raColTest_pipefd[0] waits for it's write end to close to get EOF. stdout points to the same place as it's write end, so redirecting it acts like an EOF */
		rCT_sys::test_handler(
			dup2(saved_stdout, STDOUT_FILENO),
			"dup2(saved_stdout, STDOUT_FILENO)"
		);
		logger::log(status, test_file, test_name, e.what());
		/* send logger the read end of the pipe */
		logger::log_captured_stdout(test_file, test_name, pipefd[0]);
}

void end_and_close(int* pipefd, logger::data_type status) {
	/* read end is no longer needed */
	/* This should have error checking, but it borks for some reason */
	close(pipefd[0]);
	if (status == logger::PASS) {
		printf(GRNB "p" RESET);
	} else if (status == logger::FAIL) {
		printf(REDB "F" RESET);\
	}
	else if (status == logger::ERROR) {
		printf(REDB "e" RESET);
	}
}
