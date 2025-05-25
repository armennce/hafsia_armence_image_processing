CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g # -g for debugging
LDFLAGS = -lm # Link math library for roundf, etc.

# Source files (ensure all .c files are listed)
SRCS = main.c bmp8.c bmp24.c histogram.c utils.c
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = image_processor_simple

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean all

clean:
	rm -f $(OBJS) $(TARGET)