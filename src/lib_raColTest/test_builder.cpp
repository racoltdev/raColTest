#include "test_builder.h"

#include <unistd.h>
#include <cerrno>
#include <cstdlib>
#include <string>
#include <exception>

#include "sys_utils.h"
#include "logger.h"
#include "ANSI-color-codes.h"

rCT_test::TestContext rCT_test::open_test_pipe(int* pipefd) {
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
	int saved_stdout = rCT_sys::test_handler(
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

	rCT_test::TestContext context = {
		.pipefd = pipefd,
		.saved_stdout = saved_stdout,
	};
	return context;
}

void rCT_test::build_assert(bool pass, const char* msg, rCT_test::TestContext* context) {
		/* Data may have been written but not made it through the buffer yet. Clear it */
		if (fflush(stdout) == EOF) {
			int err = errno;
			perror("Fatal error (fflush(stdout)). raColTest is exiting this test!");
			exit(err);
		}

		/* raColTest_pipefd[0] waits for it's write end to close to get EOF. stdout points to the same place as it's write end, so redirecting it acts like an EOF */
		rCT_sys::test_handler(
			dup2(context-> saved_stdout, STDOUT_FILENO),
			"dup2(saved_stdout, STDOUT_FILENO)"
		);

		if (!pass) {
			context-> status = logger::FAIL;
			logger::log(context-> status, context-> test_file, context-> test_name, msg);
			/* send logger the read end of the pipe */
			logger::log_captured_stdout(context-> test_file, context-> test_name, context-> pipefd[0]);
		}
		else {
			context-> status = logger::PASS;
			logger::log(context-> status, context-> test_file, context-> test_name, "\0");
		}
}

void rCT_test::end_and_catch(std::exception& e, rCT_test::TestContext context) {
		/* Data may have been written but not made it through the buffer yet. Clear it */
		rCT_sys::fflush_handler(
			fflush(stdout),
			"stdout"
		);
		/* raColTest_pipefd[0] waits for it's write end to close to get EOF. stdout points to the same place as it's write end, so redirecting it acts like an EOF */
		rCT_sys::test_handler(
			dup2(context.saved_stdout, STDOUT_FILENO),
			"dup2(saved_stdout, STDOUT_FILENO)"
		);
		logger::log(context.status, context.test_file, context.test_name, e.what());
		/* send logger the read end of the pipe */
		logger::log_captured_stdout(context.test_file, context.test_name, context.pipefd[0]);
}

void rCT_test::end_and_close(rCT_test::TestContext context) {
	// read end is no longer needed
	// for some reason I can't close this, but everything still works fine???
	// close(context.pipefd[0]);

	static char group = 0;
	group++;
	if (group == 5) {
		printf(" ");
		group = 0;
	}

	if (context.status == logger::PASS) {
		printf(GRNB "p" RESET);
	} else if (context.status == logger::FAIL) {
		printf(REDB "F" RESET);\
	}
	else if (context.status == logger::ERROR) {
		printf(REDB "e" RESET);
	}
}
