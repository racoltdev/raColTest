#ifndef raColTest_files_h
#define raColTest_files_h

#include <vector>
#include <string>

namespace rCT_files {
	void fcat(const char* top_path, const char* bottom_path, const char* output_path);
	std::vector<std::string> find(const char* root_dir, void (*func)(std::string));
	char* truncate_path_d(const char* path, const char* parent_path);
}

#endif
