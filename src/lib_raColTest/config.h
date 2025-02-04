#ifndef CONFIG_H
#define CONFIG_H

namespace config {
	struct Config {
		char* logfile_name;
		char* test_source_dir;
		char* test_bin_dir;
		int verbosity;
		int timeout;
		bool enable_github_status;
	};

	char* logfile_name();
	int timeout();
	char* test_source_dir();
	char* test_bin_dir();
	bool enable_github_status();
	int verbosity();
	int getConfigData(Config);

}

#endif
