#include "logger.h"

// TODO if i ever get multithreading running, this NEEDS semaphores
//#include <semaphore>
#include <time.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>

#include "sys_utils.h"
#include "ANSI-color-codes.h"
#include "config/config.h"
#include "files.h"

#define VERB(type) (type == logger::ERROR ? "encountered an exception:" : \
		type == logger::FAIL ? "failed:" : \
		type == logger::PASS ? "passed!" : \
		type == logger::STD_OUT ? "printed!" : \
		"UNKOWN VERB")

#define DELIM ":-:"
// 4kb is chosen because thats the default size of a buffer in linux kernel
#define BUFF_SIZE 4096

struct LogLine {
	time_t time;
	char test_file[64] = {};
	char test_name[64] = {};
	char* data;
	logger::data_type type;
};

const char* log_line_format = "%ld" DELIM "%s" DELIM "%d" DELIM "%s" DELIM "%s" DELIM "\n";
const char* log_path = config::logfile_name();

// Yes this is a custom parser because I didn't want to read a line in, and then split it, and then read the splits into LogLine
// That's two more passes over strings than needed!
char next_char(FILE* log_file) {
	// Checking for EOF is done in deserialize_line with (c > 0)
	char c = (char) fgetc(log_file);

	if (c != DELIM[0]) { 	// If delim not encounterd
		return c;
	} else { 				// If delim may have been encountered
		char buffer[strlen(DELIM)];
		fgets(buffer, strlen(DELIM), log_file);
		// If full delim matched
		if (!strcmp(buffer, DELIM + 1)) {
			return 0;
		} else {
			// Go back. Scanned too many characters in without reporting
			rCT_sys::error_handler(
				fseek(log_file, -1 * strlen(DELIM) + 1, SEEK_CUR),
				"Failed to seek project.log"
			);
			return c;
		}
	}
}

// line.data is a heap variable that must be freed
void map_field_to_line(LogLine* line, const char* field, short num) {
	if     (num == 0) {line-> time = strtol(field, NULL, 0);}
	else if(num == 1) {strcpy(line-> test_file, field);}
	else if(num == 2) {line-> type = static_cast<logger::data_type>(atoi(field));}
	else if(num == 3) {strcpy(line-> test_name, field);}
	else {
		line-> data = (char*) malloc(strlen(field) * sizeof(char));
	    strcpy(line-> data, field);
	}
}

// I'm not gonna do error checking here. Don't edit the log file!
// TODO stdout_cap_file will have newlines, but I don't want to move to the next line yet. Figure that out
LogLine deserialize_line(FILE* log_file) {
	LogLine output;
	short field_num = 0;
	std::string field;
	while (field_num < 5) {
		char c = next_char(log_file);
		if (c > 0) {
			field.push_back(c);
		} else { 				// If delim matched or eof
			map_field_to_line(&output, field.c_str(), field_num++);
			field = "";
		}
	}
	return output;
}

// TODO Might want to sanitize input later
// TODO may need this to work with ms precision if tests are run back to back
void logger::log(logger::data_type msg_type, const char* test_file, const char* test_name, const char* data) {
	const char* line_path = "line.log";
	FILE* line_log = rCT_sys::fopen_handler(
		fopen(line_path, "w"),
		line_path
	);
	time_t sec = time(NULL);

	fprintf(line_log, log_line_format, sec, test_file, msg_type, test_name, data);
	rCT_sys::fclose_handler(
		fclose(line_log),
		line_path
	);
	if (access(log_path, F_OK) == 0) {
		rCT_files::fcat(line_path, log_path, log_path);
		rCT_sys::io_handler(
			std::remove(line_path),
			line_path, "Failed to remove "
		);
	} else {
		rCT_sys::io_handler(
			rename(line_path, log_path),
			log_path, "Failed to rename "
		);
	}
}

void logger::log_captured_stdout(const char* test_file, const char* test_name, int stdout_cap_fd) {
	// Cap read to only ever reading int size
	int buff_cap = BUFF_SIZE;
	char* buff_d = (char*) malloc(buff_cap * sizeof(char));
	size_t buff_size = 0;
	int l = 0;
	do {

		l = rCT_sys::error_handler(
			/* Size cap to int makes this a safe type cast */
			(int) read(stdout_cap_fd, buff_d, buff_cap),
			"Could not read() from stdout_cap_fd"
		);

		buff_size += l;
		buff_cap *= 2;
		buff_d = (char*) realloc(buff_d, buff_cap * sizeof(char));
	} while (l > 0);

	// Remove extra newline caused by fflush
	if (buff_size <= 0) {
		buff_d[0] = '\0';
	} else {
		buff_d[buff_size - 1] = '\0';
	}
	logger::log(logger::STD_OUT, test_file, test_name, buff_d);
	free(buff_d);
}

std::vector<LogLine> lines_in_range(time_t start_time, time_t end_time) {
	std::vector<LogLine> lines;
	FILE* log_file = rCT_sys::fopen_handler(
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

void print_line(LogLine l, bool show_stdout) {
	printf("Test \"%s\" %s %s\n", l.test_name, VERB(l.type), l.data);
	if (show_stdout) {
		printf(REDB "Captured stdout----------------" RESET "\n");
	}
}

bool no_verbosity_printer(LogLine l, bool) {
	return (l.type >= 2) ? true : false;
}

bool error_verbosity_printer(LogLine l, bool show_stdout) {
	if (show_stdout) {
		show_stdout = false;
		if (l.type != logger::STD_OUT) {
			printf(YELB "No captured standard out-------" RESET "\n\n");
			if (l.type < 2) {
				print_line(l, true);
				show_stdout = true;
			}
		} else if (l.data[0] == '\0') {
			// If stdout field is empty, don't bother printing it
			printf(YELB "No captured standard out-------" RESET "\n\n");
		} else {
			printf("%s\n" YELB "End of captured stdout---------" RESET "\n\n", l.data);
		}
	}
	else if (l.type < logger::PASS) {
		print_line(l, true);
		show_stdout = true;
	}
	return show_stdout;
}

bool full_verbosity_printer(LogLine l, bool show_stdout) {
	if (show_stdout) {
		if (l.type == logger::STD_OUT) {
			printf("%s\n" YELB "End of captured stdout---------" RESET "\n\n", l.data);
			return false;
		} else {
			printf(YELB "No captured standard out-------" RESET "\n\n");
		}
	}

	show_stdout = (l.type < logger::PASS) ? true : false;
	print_line(l, show_stdout);
	return show_stdout;
}

bool logger::display(time_t start_time, time_t end_time) {
	std::vector<LogLine> lines = lines_in_range(start_time, end_time);

	bool pass = true;
	bool (*verbosity_printer) (LogLine, bool);
	if (config::verbosity() == 0) {
		verbosity_printer = &no_verbosity_printer;
	} else if (config::verbosity() == 1) {
		verbosity_printer = &error_verbosity_printer;
	} else if (config::verbosity() == 2) {
		verbosity_printer = &full_verbosity_printer;
	}
	bool show_stdout = false;
	// pre-decrement is done on purpose since i is unsigned
	for (size_t i = lines.size(); i > 0; --i) {
		LogLine l = lines[i - 1];
		if (l.type < logger::PASS) {
			pass = false;
		}
		show_stdout = verbosity_printer(l, show_stdout);
		// Finally free l.data.
		// This is hard to track. Maybe use an arena?
		free(l.data);
	}
	// In case there was no stdout and that was the last line in range
	if (show_stdout && config::verbosity() > 0) {
		printf(YELB "No captured standard out-------" RESET "\n\n");
	}

	return pass;
}
