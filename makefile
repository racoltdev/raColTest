TARGET := raColTest
BUILD_DIR := build/
SOURCE_DIR := src/
TEST_DIR := testsrc/
INCLUDE_DIRS :=
LIBS :=
CXX := gcc


LDFLAGS := $(CXXFLAGS) $(addprefix -l, $(LIBS))
CXXFLAGS += -MMD -MP -I$(SOURCE_DIR) $(addprefix -I, $(INCLUDE_DIRS))
THIS_MAKEFILE := $(firstword $(MAKEFILE_LIST))
SOURCES != find $(SOURCE_DIR) -name '*.c' ! -wholename 'src/main.c'
OBJECTS := ${SOURCES:%.c=$(BUILD_DIR)%.o}
TEST_SOURCES != find $(TEST_DIR) -name '*.c'
TESTS := ${TEST_SOURCES:$(TEST_DIR)%.c=tests/%}
DEPFILES := ${OBJECTS:%.o=%.d}
-include $(DEPFILES)

$(BUILD_DIR)%.o: %.c $(THIS_MAKEFILE)
	@mkdir -p "${@D}"
	@echo "[CXX] ${@F}"
	@$(CXX) $(CXXFLAGS) -c "$<" -o "$@"

tests/%: $(BUILD_DIR)$(TEST_DIR)%.o $(OBJECTS)
	@echo "[CXX] ${@F}"
	@$(CXX) $^ $(LDFLAGS) -o $@

$(TARGET): $(OBJECTS) $(BUILD_DIR)src/main.o
	@echo "[LD] ${@F}"
	@$(CXX) $^ $(LDFLAGS) -o $(TARGET)

all: $(TARGET) $(TESTS)

clean:
	rm -rf $(TARGET)
	rm -rf $(BUILD_DIR)

.PHONY: clean all
