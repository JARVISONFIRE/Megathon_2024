# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g -I include  # Adding -I include to specify include directory

# Directories
SRC_DIR = src
LIB_DIR = lib
INC_DIR = include

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(LIB_DIR)/*.c)

# Output executable
OUTPUT = game

# Default target
all: $(OUTPUT)

# Link object files to create the executable
$(OUTPUT): $(SRC_FILES)
	$(CC)	$(CFLAGS)	-o	$(OUTPUT)	$(SRC_FILES)

# Clean up build files
clean:
	rm	-f	$(OUTPUT)

.PHONY: all clean