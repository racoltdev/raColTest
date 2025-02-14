#ifndef CONFIG_H
#define CONFIG_H

namespace config {
	const char* logfile_name();
	int timeout();
	const char* test_source_dir();
	const char* test_bin_dir();
	bool enable_github_status();
	int verbosity();
}

#endif
