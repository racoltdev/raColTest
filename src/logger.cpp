#include "logger.h"

// TODO if i ever get multithreading running, this NEEDS semaphores
//#include <semaphore>
#include <time.h>
#include <stdio.h>
#include <vector>

#include "sys_utils.h"

#define DELIM ":-:"
#define BUFF_SIZE 64

struct LogLine {
	time_t time;
	char test_file;
	logger::data_type type;
	char test_name;
	char* data;
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
		if (strcmp(buffer, DELIM + 1)) {
			return NULL;
		} else {
			fseek(log_file, -2, SEEK_CUR);
			return c;
		}
	}
}

void map_field_to_line(LogLine line, char* field, short num) {
	num == 0 ? line.time = strtol(field) :
	num == 1 ? line.test_file = field    :
	num == 2 ? line.type = atoi(field)   :
	num == 3 ? line.test_name = field    :
	           line.data = field;
}

// I'm not gonna do error checking here. Don't edit the log file!
LogLine deserialize_line(FILE* log_file, long int line_len) {
	LogLine output;

	for (int i = 0; i < line_len; i++) {
		std::vector<char> field;
		//if (field > 4) {
		//	throw std::runtime_error("Invalid log file! Too many fields encountered");
		//}
		char c = next_char(log_file);
		if (c != NULL) {
			field.push_back(c);
		} else {
			map_field_to_line(output, field, i);
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

// TODO Might want to sanitize input later
// TODO may need this to work with ms precision if tests are run back to back
void logger::log(logger::data_type msg_type,  char* test_file, const char* test_name, const char* data) {
	FILE* log_file = fopen(log_path, "a");
	time_t sec = time(NULL);
	fprintf(log_file, log_line_format, sec, test_file, msg_type, test_name, data);
	fclose(log_file);
}

// TODO every fseek is a very expensive operation bc it wipes the stdio buffer. Consider replacing fseek, searching in order, or using a more conventional parser to avoid this
void logger::display(time_t start_time, time_t end_time) {
	std::vector<LogLine> lines;
	FILE* log_file = fopen(log_path, "r");
	rCT_sys::error_handler( \
		fseek(log_file, 0, SEEK_END), \
		"Could not seek to position in log file" \
	);
	// Assume future issues with calling fseek are because I'm at the start or end of file. AKA not catastrophic and I don't need to exit
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
