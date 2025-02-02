TARGET := raColTest
LIB := lib_$(TARGET)

BUILD_DIR := build/
SOURCE_DIR := src/
LIB_DIR := $(SOURCE_DIR)lib_raColTest/
TEST_DIR := test/
TEST_BINDIR := testbin/

CXXFLAGS := --std=c++20 -g3 -Wall -Wextra -Wpedantic -Wconversion -fPIC
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

$(TEST_BINDIR)%: $(BUILD_DIR)$(TEST_DIR)%.o $(OBJECTS)
	@echo "[CXX] ${@F}"
	@mkdir -p $(TEST_BINDIR)
	@$(CXX) $^ $(CXXFLAGS) -o "$@"

$(TARGET): $(OBJECTS) $(BUILD_DIR)src/main.o
	@echo "[LD] ${@F}"
	@$(CXX) $^ $(CXXFLAGS) -o $(TARGET)

$(LIB): $(LIB_OBJECTS)
	@echo "[SO] ${@F}"
	@$(CXX) $(LD_FLAGS) $^ -o "$@.so"

all: $(TARGET) $(TESTS)

install: $(TARGET) $(LIB)
	@cp $(TARGET) /usr/local/bin/$(TARGET)
	@cp $(LIB).so /usr/lib/$(LIB).so

uninstall:
	rm /usr/local/bin/$(TARGET)
	rm /usr/lib/$(LIB).so

clean:
	rm -rf $(TARGET)
	rm -rf $(BUILD_DIR)
	rm -rf $(TEST_BINDIR)

.PHONY: clean all
