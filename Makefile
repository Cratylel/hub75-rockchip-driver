# Compiler and flags
CXX ?= g++
CXXFLAGS := -Wall -Wextra -O3 -std=c++17 -g -rdynamic
CXXFLAGS += $(if $(TEST_ENV),-DTEST_ENV)
CXXFLAGS += $(if $(LOG_LEVEL_DEBUG),-DLOG_LEVEL_DEBUG)
CXXFLAGS += $(if $(TEST_SERVER),-DTEST_SERVER)

# Directories
SRCDIR := lib
OBJDIR := build

# Source and object files
SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

# Output binary
TARGET ?= main_x86_64

# Default rule
all: $(TARGET)

# Linking the final binary
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

# Compiling source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure build directory exists
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean rule
clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean
