#include "logger.h"

// TODO if i ever get multithreading running, this NEEDS semaphores
//#include <semaphore>
#include <time.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <string>
#include <filesystem>
#include <unistd.h>
#include <fcntl.h>

#include "sys_utils.h"
#include "ANSI-color-codes.h"

#define VERB(type) (type == logger::ERROR ? "encountered an exception:" : \
		type == logger::FAIL ? "failed:" : \
		"UNKOWN VERB")

#define DELIM ":-:"
// 4kb is chosen because thats the default size of a buffer in linux kernel
#define BUFF_SIZE 4096

// TODO made data variable size
struct LogLine {
	time_t time;
	char test_file[64] = {};
	logger::data_type type;
	char test_name[64] = {};
	char data[128] = {};
};

const char* log_line_format = "%ld" DELIM "%s" DELIM "%d" DELIM "%s" DELIM "%s" DELIM "\n";
const char* log_path = "project.log";

// Yes this is a custom parser because I didn't want to read a line in, and then split it, and then read the splits into LogLine
// That's two more passes over strings than needed!
char next_char(FILE* log_file) {
	char c = fgetc(log_file);
	// If delim not encounterd
	if (c != DELIM[0]) {
		return c;
	}
	// If delim may have been encountered
	else {
		char buffer[strlen(DELIM)];
		fgets(buffer, strlen(DELIM), log_file);
		// If full delim matched
		if (!strcmp(buffer, DELIM + 1)) {
			return 0;
		} else {
			rCT_sys::error_handler( \
				fseek(log_file, -2, SEEK_CUR), \
				"Failed to seek project.log" \
			);
			return c;
		}
	}
}

void map_field_to_line(LogLine* line, const char* field, short num) {
	if     (num == 0) {line->time = strtol(field, NULL, 0);}
	else if(num == 1) {strcpy(line->test_file, field);}
	else if(num == 2) {line->type = static_cast<logger::data_type>(atoi(field));}
	else if(num == 3) {strcpy(line->test_name, field);}
	else              {strcpy(line->data, field);}
}

// I'm not gonna do error checking here. Don't edit the log file!
// TODO stdout_cap_file will have newlines, but I don't want to move to the next line yet. Figure that out
LogLine deserialize_line(FILE* log_file) {
	LogLine output;
	int field_num = 0;
	std::string field;
	while (field_num < 5) {
		char c = next_char(log_file);
		if (c > 0) {
			field.push_back(c);
		} else {
			map_field_to_line(&output, field.c_str(), field_num++);
			field = "";
		}
	}
	return output;
}

void fcat(const char* top_path, const char* bottom_path, const char* output_path) {
	const char* temp_path = "temp_cat.log";
	if (std::filesystem::exists(temp_path)) {
		std::remove(temp_path);
	}
	std::filesystem::copy(top_path, temp_path);

	FILE* bottom_file = rCT_sys::fopen_handler( \
		fopen(bottom_path, "r"), \
		bottom_path \
	);
	FILE* temp_log = rCT_sys::fopen_handler( \
		fopen(temp_path, "a"), \
		temp_path \
	);

	char* buff[BUFF_SIZE];
	size_t read_size;
	do {
		read_size = fread(buff, sizeof(char), BUFF_SIZE, bottom_file);
		fwrite(buff, sizeof(char), read_size, temp_log);
		rCT_sys::error_handler(errno, "Failed to write to temp_cat.log");
	} while (read_size == BUFF_SIZE);
	rCT_sys::error_handler(errno, "Failed to read from project.log");
	rCT_sys::fclose_handler( \
		fclose(bottom_file), \
		bottom_path
	);
	rCT_sys::fclose_handler( \
		fclose(temp_log), \
		temp_path \
	);
	std::filesystem::rename(temp_path, output_path);
}

// TODO Might want to sanitize input later
// TODO may need this to work with ms precision if tests are run back to back
void logger::log(logger::data_type msg_type, const char* test_file, const char* test_name, const char* data) {
	const char* line_path = "line.log";
	FILE* line_log = rCT_sys::fopen_handler( \
		fopen(line_path, "w"), \
		line_path \
	);
	time_t sec = time(NULL);

	fprintf(line_log, log_line_format, sec, test_file, msg_type, test_name, data);
	rCT_sys::fclose_handler( \
		fclose(line_log), \
		line_path \
	);
	if (std::filesystem::exists(log_path)) {
		fcat(line_path, log_path, log_path);
		rCT_sys::io_handler( \
			std::remove(line_path), \
			line_path, "Failed to remove " \
		);
	} else {
		std::filesystem::rename(line_path, log_path);
	}
}

void logger::log_captured_stdout(const char* test_file, const char* test_name, int stdout_cap_fd) {
	size_t buff_cap = BUFF_SIZE;
	char* buff = (char*) malloc(buff_cap * sizeof(char));
	size_t buff_size = 0;
	size_t l = 0;
	do {
		l = rCT_sys::error_handler( \
			read(stdout_cap_fd, buff, buff_cap), \
			"Could not read() from stdout_cap_fd"
		);
		buff_size += l;
		char* temp = buff;
		buff_cap *= 2;
		char* buff = (char*) malloc(buff_cap * sizeof(char));
		strcpy(buff, temp);
	} while (l > 0);
	// Remove extra newline caused by fflush
	if (buff_size <= 0) {
		buff[0] = '\0';
	} else {
		buff[buff_size - 1] = '\0';
	}
	logger::log(logger::STD_OUT, test_file, test_name, buff);
	free(buff);
}

std::vector<LogLine> lines_in_range(time_t start_time, time_t end_time) {
	std::vector<LogLine> lines;
	FILE* log_file = rCT_sys::fopen_handler( \
		fopen(log_path, "r"),
		log_path
	);
	do {
		LogLine l = deserialize_line(log_file);
		if (l.time <= end_time) {
			lines.push_back(l);
		}
	} while(lines.back().time >= start_time);
	fclose(log_file);
	lines.pop_back();
	return lines;
}

void logger::display(time_t start_time, time_t end_time) {
	std::vector<LogLine> lines = lines_in_range(start_time, end_time);
	bool show_stdout = false;
	bool pass = true;
	for (size_t i = lines.size(); i > 0; --i) {
		LogLine l = lines[i - 1];
		if (show_stdout) {
			show_stdout = false;
			if (l.type != logger::STD_OUT) {
				printf(YELB "No captured standard out-------" RESET "\n\n");
			} else {
				printf("%s\n" YELB "End of captured stdout---------" RESET "\n\n", l.data);
			}
		}
		else if (l.type < logger::PASS) {
			pass = false;
			printf("Test \"%s\" %s %s\n", l.test_name, VERB(l.type), l.data);
			printf(REDB "Captured stdout----------------" RESET "\n");
			show_stdout = true;
		}
	}
	// In case there was no stdout and that was the last line in range
	if (show_stdout) {
		printf(YELB "No captured standard out-------" RESET "\n\n");
	}

	if (pass) {
		printf(GRNB "All tests passed successfully.\nBuild successful!" RESET "\n");
	} else {
		printf(REDB "Some tests failed!" RESET "\n");
	}
}
