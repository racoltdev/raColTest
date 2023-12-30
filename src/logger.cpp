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

#define DELIM ":-:"
#define BUFF_SIZE 4000

// TODO check if using windows and swap with windows text coloring conio.h
#define REDB "\e[0;101m"
#define GRNB "\e[42m"
#define RESET "\x1B[0m"

// TODO made data variable size
struct LogLine {
	time_t time;
	char test_file[64] = {};
	logger::data_type type;
	char test_name[64] = {};
	char data[128] = {};
};

const char* log_line_format = "%ld" DELIM "%s" DELIM "%d" DELIM "%s" DELIM "%s\n";
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
			fseek(log_file, -2, SEEK_CUR);
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
	while (field_num < 6) {
		char c = next_char(log_file);
		if (c > 0 && c != '\n') {
			field.push_back(c);
		} else {
			map_field_to_line(&output, field.c_str(), field_num++);
			field = "";
		}
	}
	return output;
}

// 4kb is chosen because thats the default size of a buffer in linux kernel
void fcat(const char* top_path, const char* bottom_path, const char* output_path) {
	const char* temp_path = "temp_cat.log";
	// Save errno state and restore because this error is useless and muddying up debugging
	int save_errno = errno;
	// In case program crash before renaming temp_path, remove it to make copy happy
	// copy crashes it 'to' exists
	if (std::filesystem::exists(temp_path)) {
		std::remove(temp_path);
	}
	// Undocumented behavior of filesystem::copy:
	// 	If temp path does not exist, sets errno to 'No such file or directory'
	std::filesystem::copy(top_path, temp_path);
	if (errno == ENOENT) {
		errno = save_errno;
	}

	FILE* bottom_file = fopen(bottom_path, "r");
	FILE* temp_log = fopen(temp_path, "a");

	char* buff[BUFF_SIZE];
	while (fread(buff, sizeof(char), BUFF_SIZE, bottom_file) == BUFF_SIZE) {
		fwrite(buff, sizeof(char), BUFF_SIZE, temp_log);
	}
	fclose(bottom_file);
	fclose(temp_log);
	std::filesystem::rename(temp_path, output_path);
}

// TODO Might want to sanitize input later
// TODO may need this to work with ms precision if tests are run back to back
void logger::log(logger::data_type msg_type, const char* test_file, const char* test_name, const char* data) {
	const char* line_path = "line.log";
	FILE* line_log = fopen(line_path, "w");
	time_t sec = time(NULL);

	fprintf(line_log, log_line_format, sec, test_file, msg_type, test_name, data);
	fclose(line_log);
	fcat(line_path, log_path, log_path);
	rCT_sys::io_handler(std::remove(line_path), line_path, "Failed to remove ");
}

void logger::log_captured_stdout(const char* test_file, const char* test_name, int stdout_cap_fd) {
	size_t buff_size = 4096;
	char* buff = (char*) calloc(buff_size, sizeof(char));
	while (read(stdout_cap_fd, buff, buff_size) > 0) {
		char* temp = buff;
		buff_size *= 2;
		char* buff = (char*) calloc(buff_size, sizeof(char));
		strcpy(buff, temp);
	}
	logger::log(logger::STD_OUT, test_file, test_name, buff);
	free(buff);
}

void logger::display(time_t start_time, time_t end_time) {
	std::vector<LogLine> lines;
	FILE* log_file = fopen(log_path, "r");
	do {
		LogLine l = deserialize_line(log_file);
		if (l.time <= end_time) {
			lines.push_back(l);
		}
	} while(lines.back().time >= start_time);

	fclose(log_file);
}
