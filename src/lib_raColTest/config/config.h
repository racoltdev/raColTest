#ifndef CONFIG_H
#define CONFIG_H

namespace config {
	const char* logfile_name();
	int timeout();
	const char* test_source_dir();
	const char* test_bin_dir();
	bool enable_github_status();
	int verbosity();

	struct Config {
		const char* logfile_name;
		const char* test_source_dir;
		const char* test_bin_dir();
		int timeout;
		int verbosity;
		bool enable_github_status;
	};
}

#endif
