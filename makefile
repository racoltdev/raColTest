TARGET_NAME := raColTest
BUILD_DIR := build
TEST_EXECUTABLE := run_test
# report, quiet on pass, Update coverage
TEST_ARGS := -rqU

CC := gcc
SOURCE_DIRECTORIES := src/
TESTS_DIRECTORIES := tests/
INCLUDE := ${SOURCE_DIRECTORIES:%=-I%} ${TESTS_DIRECTORIES:%=-I%}

SOURCES := $(shell for f in $(SOURCE_DIRECTORIES); do find $$f | grep '\.c'; done)
OBJECTS := $(SOURCES:%.c=$(BUILD_DIR)/%.o)
TEST_SOURCES := $(shell for f in $(TESTS_DIRECTORIES); do find $$f | grep '\.c'; done)
TEST_OBJECTS := $(TEST_SOURCES:%.c=$(BUILD_DIR)/%.o)
DEPFILES := $(OBJECTS:%.o=%.d) $(TEST_OBJECTS:%.o=%.d)
-include $(DEPFILES)

$(BUILD_DIR)/tests/%.o: $(TESTS_DIRECTORIES)%.c
	@echo '[CXX] $(<F)'
	@mkdir -p '$(@D)'
	$(CC) $(INCLUDE) -MMD -MP -c '$<' -o '$@'

build_tests: $(TEST_OBJECTS)
	@echo '[LD] Linking test suite........]'
	$(CC) $(TEST_OBJECTS) -o $(TEST_EXECUTABLE)

test: build_tests
	@echo '[Running tests.................]'
	# Tests must log data. All logs should be marked as PASS/FAIL and
	# test type: failed compilation, local test run, attempted commit, commit, remote push
	$(TEST_EXECUTABLE) $(TEST_ARGS) $(BUILD_DIR)

$(BUILD_DIR)/src/%.o: $(SOURCE_DIRECTORIES)%.c
	@echo '[CXX] $(<F)'
	@mkdir -p '$(@D)'
	$(CC) $(INCLUDE) -MMD -MP -c '$<' -o '$@'

$(TARGET_NAME): $(OBJECTS)
	@echo '[Linking.......................]'
	$(CC) $(OBJECTS) -o $(TARGET_NAME)

all: $(TARGET_NAME) test

clean:
	rm -rf build/

# .SILENT:
