# Compiler and target name
CXX := g++
TARGET := ofce

# Base compiler, preprocessor, and linker flags
CXXFLAGS := -std=c++23 -Wall -Wextra -Wshadow -pedantic -pthread
CPPFLAGS := -MMD -MP
LDFLAGS := -pthread

# Enable debug flags if DEBUG=1 is set, otherwise use optimization flags
ifeq ($(DEBUG),1)
	CXXFLAGS += -O0 -g
else
	CXXFLAGS += -O3 -flto=auto -funroll-loops -DNDEBUG
	LDFLAGS += -flto=auto
endif

# Source files and corresponding object files
SRCS := $(wildcard src/*.cpp)
BUILD_DIR := build
OBJS := $(patsubst src/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
DEPS := $(OBJS:.o=.d)

# Declare phony targets
.PHONY: all clean

# Default target
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

# Create the build directory if it doesn't exist
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Compile source files into object files
$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# Include dependency tracking files
-include $(DEPS)

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)