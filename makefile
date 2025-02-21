TARGET := raColTest
LIB := lib_$(TARGET)
INSTALL_BINDIR := /usr/local/bin/
INSTALL_LIBDIR := /usr/lib/

BUILD_DIR := build/
SOURCE_DIR := src/
LIB_DIR := $(SOURCE_DIR)lib_raColTest/
CONFIG_PATH := /etc/$(TARGET)/
TEST_DIR := test/
TEST_BINDIR := testbin/

CXXFLAGS := --std=c++20 -g3 -Wall -Wextra -Wpedantic -Wconversion -Wno-missing-field-initializers -fPIC
CXXFLAGS += -MMD -MP -I$(SOURCE_DIR)
LD_FLAGS := -shared

THIS_MAKEFILE := $(firstword $(MAKEFILE_LIST))

SOURCES != find $(SOURCE_DIR) -name '*.cpp' ! -wholename 'src/main.cpp'
OBJECTS := ${SOURCES:%.cpp=$(BUILD_DIR)%.o}
LIB_OBJECTS != find $(BUILD_DIR)$(LIB_DIR) -name '*.o'
TEST_SOURCES != find $(TEST_DIR) -name '*.cpp'
TESTS := ${TEST_SOURCES:$(TEST_DIR)%.cpp=$(TEST_BINDIR)%}
DEPFILES := ${OBJECTS:%.o=%.d}
-include $(DEPFILES)

$(BUILD_DIR)%.o: %.cpp $(THIS_MAKEFILE)
	@mkdir -p "${@D}"
	@echo "[CXX] ${@F}"
	@$(CXX) $(CXXFLAGS) -c "$<" -o "$@"

TEST_BIN_TREE:
	@find test/* -type d | cut -d/ -f2- | xargs printf -- "testbin/%s\n" | xargs mkdir -p
	@echo "[mkdir] TEST_BINDIR tree"

$(TEST_BINDIR)%: $(BUILD_DIR)$(TEST_DIR)%.o $(OBJECTS)
	@echo "[CXX] ${@F}"
	@mkdir -p $(TEST_BINDIR)
	@$(CXX) $^ $(CXXFLAGS) -o "$@"

$(TARGET): $(OBJECTS) $(BUILD_DIR)src/main.o
	@echo "[LD] ${@F}"
	@$(CXX) $^ $(CXXFLAGS) -o $(TARGET)

$(LIB): $(LIB_OBJECTS)
	@echo "[SO] ${@F}"
	@$(CXX) $(LD_FLAGS) $^ -o $(BUILD_DIR)$@.so

all: TEST_BIN_TREE $(TARGET) $(TESTS)

install: $(TARGET) $(LIB)
	cp $(TARGET) $(INSTALL_BINDIR)$(TARGET)
	cp $(BUILD_DIR)$(LIB).so $(INSTALL_LIBDIR)$(LIB).so
	@mkdir -p $(CONFIG_PATH)
	cp $(LIB_DIR)config/defaults.config $(CONFIG_PATH)

uninstall:
	rm $(INSTALL_BINDIR)$(TARGET)
	rm $(INSTALL_LIBDIR)$(LIB).so
	rm $(CONFIG_PATH)defaults.config

clean:
	rm -rf $(TARGET)
	rm -rf $(BUILD_DIR)
	rm -rf $(TEST_BINDIR)

.PHONY: clean all
