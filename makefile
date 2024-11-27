# Compile
CC = gcc

# Compiler flags
#CFLAGS = -Wall -Wextra -std=c11 -Iinclude
CFLAGS = -Wall -std=c11 -Iinclude

LDFLAGS = -lm

# Target executable
TARGET = main 

# Source directories
SRCDIR = src
INCDIR = include

# Source files
SRCS = $(SRCDIR)/main.c $(SRCDIR)/memory.c

# Header files
HEADERS = $(INCDIR)/memory.h

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link object files to create the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile each source file into an object file
$(SRCDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the build directory
clean:
	rm -f $(OBJS)

# Phony targets
.PHONY: all clean

