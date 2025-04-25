# Compiler
CC = gcc

# Parameters
CFLAGS = -Wall -O2

# Sources and objects
SRCS = ls3w2iq.c ini_parser.c ls3w_decode.c
OBJS = $(SRCS:.c=.o)

# Target
TARGET = ls3w2iq

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c -> .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean Up
clean:
	rm -f *.o $(TARGET)
