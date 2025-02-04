#ifndef CONFIG_H
#define CONFIG_H

namespace config {
	char* logfile_name();
	int timeout();
	char* test_source_dir();
	char* test_bin_dir();
	bool enable_github_status();
	int verbosity();
}

#endif
