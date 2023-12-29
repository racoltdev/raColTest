#include "logger.h"

// TODO if i ever get multithreading running, this NEEDS semaphores
//#include <semaphore>
#include <time.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <filesystem>

#include "sys_utils.h"

#define DELIM ":-:"
#define BUFF_SIZE 4000

struct LogLine {
	time_t time;
	const char* test_file;
	logger::data_type type;
	const char* test_name;
	const char* data;
};

const char* log_line_format = "%ld" DELIM "%s" DELIM "%d" DELIM "%s" DELIM "%s\n";
const char* log_path = "./project.log";

// Yes this is a custom parser because I didn't want to read a line in, and then split it, and then read the splits into LogLine
// That's two more passes over strings than needed!
char next_char(FILE* log_file) {
	char c;
	// If delim not encounterd
	if ((c = fgetc(log_file)) != DELIM[0]) {
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

void map_field_to_line(LogLine line, const char* field, short num) {
	if(num == 0) {line.time = strtol(field, NULL, 0);}
	if(num == 1) {line.test_file = field;}
	if(num == 2) {line.type = static_cast<logger::data_type>(atoi(field));}
	if(num == 3) {line.test_name = field;}
	else {line.data = field;}
}

// I'm not gonna do error checking here. Don't edit the log file!
LogLine deserialize_line(FILE* log_file, long int line_len) {
	LogLine output;

	for (int i = 0; i < line_len; i++) {
		std::string field;
		//if (field > 4) {
		//	throw std::runtime_error("Invalid log file! Too many fields encountered");
		//}
		char c = next_char(log_file);
		if (c != 0) {
			field.append(&c);
		} else {
			map_field_to_line(output, field.c_str(), i);
		}
	}
	printf("%s, %s\n", output.test_file, output.test_name);
	return output;
}

int seek_prev_line(FILE* log_file) {
	// always seek back before scan bc current char is expected to be '\n'
	int status = 0;
	do {
		status = fseek(log_file, -2, SEEK_CUR);
	} while(fgetc(log_file) != '\n' && status == 0);
	return status;
}

// 4kb is chosen because thats the default size of a buffer in linux kernel
void fcat(const char* top_path, const char* bottom_path, const char* output_path) {
	const char* temp_path = "temp_cat.log";
	std::filesystem::copy(top_path, temp_path);

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

// TODO every fseek is a very expensive operation bc it wipes the stdio buffer. Consider replacing fseek, searching in order, or using a more conventional parser to avoid this
void logger::display(time_t start_time, time_t end_time) {
	std::vector<LogLine> lines;
	FILE* log_file = fopen(log_path, "r");
	long int next_end_pos = ftell(log_file);
	do {
		int status = seek_prev_line(log_file);
		// Scanned to start of log .... most likely
		if (status != 0) {
			break;
		}

		long int start_pos = ftell(log_file);
		long int line_len = start_pos - next_end_pos;
		LogLine l = deserialize_line(log_file, line_len);
		next_end_pos = start_pos - 1;

		if (l.time <= end_time) {
			lines.push_back(l);
		}

		long int end_pos = ftell(log_file);
		long int offset = start_pos - end_pos;
		fseek(log_file, offset, SEEK_CUR);
	} while(lines.back().time >= start_time);

	fclose(log_file);
}
