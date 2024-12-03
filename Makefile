# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -O2

# Directories
SRC_DIR = src
BUILD_DIR = build
EXEC = breakout

# SFML Configuration
SFML_INC = /path/to/sfml/include
SFML_LIB = /path/to/sfml/lib
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Sources and objects
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Default target
all: $(EXEC)

# Build executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -I$(SFML_INC) -o $@ $^ -L$(SFML_LIB) $(SFML_FLAGS)

# Build objects
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I$(SFML_INC) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build
clean:
	rm -rf $(BUILD_DIR) $(EXEC)

# Phony targets
.PHONY: all clean
