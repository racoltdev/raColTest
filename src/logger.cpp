#include "logger.h"

#include <semaphore>
#include <time.h>

#define DELIM ":-:"

// TODO Might want to sanitize input later
void logger::log(logger::data_type msg_type,  char* test_file, const char* test_name, const char* data) {
	const char* log_path = "./project.log";
	FILE* log_file = fopen(log_path, "a");
	time_t sec = time(NULL);
	fprintf(log_file, "%ld" DELIM "%s" DELIM "%d" DELIM "%s" DELIM "%s\n" , sec, test_file, msg_type, test_name, data);
	fclose(log_file);
}
