#ifndef CONFIG_H
#define CONFIG_H

namespace config {
	extern const char* logfile_name;
	extern const int timeout;
	extern const char* test_source_dir;
	extern const char* test_bin_dir;
	extern const bool enable_github_status;
	extern const int verbosity;
}

#endif
