#include "files.h"

#include <filesystem>

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
}
