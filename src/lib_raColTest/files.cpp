#include "files.h"

#include <filesystem>
#include <string.h>

#include "sys_utils.h"

#define BUFF_SIZE 4096

namespace rCT_files {
	void fcat(const char* top_path, const char* bottom_path, const char* output_path) {
		const char* temp_path = "temp_cat.txt";
		char* err_msg_d = rCT_sys::good_strcat("Failed to read from ", bottom_path);

		if (std::filesystem::exists(temp_path)) {
			std::remove(temp_path);
		}
		std::filesystem::copy(top_path, temp_path);

		FILE* bottom_file = rCT_sys::fopen_handler(
			fopen(bottom_path, "r"),
			bottom_path
		);
		FILE* temp_log = rCT_sys::fopen_handler(
			fopen(temp_path, "a"),
			temp_path
		);

		char* buff[BUFF_SIZE];
		size_t read_size;
		do {
			read_size = fread(buff, sizeof(char), BUFF_SIZE, bottom_file);
			fwrite(buff, sizeof(char), read_size, temp_log);
			rCT_sys::error_handler(errno, "Failed to write to temp_cat.txt");
		} while (read_size == BUFF_SIZE);
		rCT_sys::error_handler(errno, err_msg_d);
		free(err_msg_d);

		rCT_sys::fclose_handler(
			fclose(bottom_file),
			bottom_path
		);
		rCT_sys::fclose_handler(
			fclose(temp_log),
			temp_path
		);
		std::filesystem::rename(temp_path, output_path);
	}

	std::vector<std::string> find(const char* root_dir, void (*func)(std::string)) {
		std::vector<std::string> file_names;
		for (const auto& dir : std::filesystem::recursive_directory_iterator(root_dir)) {
			if (dir.is_regular_file()) {
					const std::string p = std::filesystem::absolute(dir.path());
					file_names.push_back(p);
					func(p);
			}

		}
		return file_names;
	}

	// Deprecated. Use truncate_path_d instead
	// parents: How many parents of the base filename should be included in the returned string
	// 0 means just the filename itself
	char* isolate_fname_d(char* path, int parents) {
		parents++;
		int children = 0;
		long int fname_start = 0;
		long int path_len = strlen(path);
		char delim = '/';
		// Always safe to start search at [-2]
		for (long int i = path_len - 2; i >= 0; i--) {
			if (path[i] == delim) {
				fname_start = i + 1;
				if (++children == parents) {
					break;
				}
			}
		}
		size_t fname_size = path_len - fname_start;
		char* fname_d = (char*) malloc(sizeof(char) * fname_size);
		strcpy(fname_d, path + fname_start);
		return fname_d;
	}

	char* truncate_path_d(const char* path, const char* parent_path) {
		char* full_path_d = realpath(path, NULL);
		char* full_parent_d = realpath(parent_path, NULL);
		char* ret_d = (char*) malloc(sizeof(char) * (strlen(full_path_d) - strlen(full_parent_d)) + 1);
		strcpy(ret_d, full_path_d + strlen(full_parent_d) + 1);
		free(full_path_d);
		free(full_parent_d);
		return ret_d;
	}
}
