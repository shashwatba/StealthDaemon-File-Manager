# Compiler and archiver settings
CC = g++
AR = ar
CFLAGS = -Wall -c
ARFLAGS = rcs

# Object files
OBJ = Wad.o

# Library
LIB = libWad.a

# Build the library
$(LIB): $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

# Compile source files into object files
Wad.o: Wad.cpp Wad.h NaryTree.h
	$(CC) $(CFLAGS) -o $@ Wad.cpp

# Clean rule to clean up object files and the library
clean:
	rm -f $(OBJ) $(LIB)

.PHONY: clean
