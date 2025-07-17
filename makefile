# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Source files
SRCS = partb/main.c partb/split.c partb/log.c partb/process.c partb/hop.c partb/reveal.c partb/proclore.c partb/seek.c partb/act.c partb/fgbg.c partb/io.c partb/neonate.c partb/signals.c partb/man.c

# Object files (replace .c with .o in the SRCS list)
OBJS = $(SRCS:.c=.o)

# Executable name
EXEC = a.out

# Default target
all: $(EXEC)

# Linking object files to create the executable
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS)

# Compiling .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(OBJS) $(EXEC)

# Run the shell
run: $(EXEC)
	./$(EXEC)

# Phony targets
.PHONY: all clean run
