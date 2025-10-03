# --- Compiler and flags ---
CC      = gcc
LIBS    = -lcurl
CFLAGS  = -Wall -Wextra -Iinclude -MMD -MP
# -Wall   	= enable common warnings
# -Wextra 	= enable extra warnings
# -Werror 	= treat warnings as errors
# -Iinclude = look for header files in ./include

# Debug option
ifeq ($(MODE),debug)
	CFLAGS += -g -O0
	OUTDIR := build/debug
else
	OUTDIR := build/release
endif

# --- Source and object files ---
SRC     = $(wildcard src/*.c)                           # Expands to all files in the filesystem that match the pattern.
OBJ     = $(patsubst src/%.c, $(OUTDIR)/%.o, $(SRC))    # "src/foo.c" -> "build/foo.o" ((patsubst PATTERN, REPLACEMENT, TEXT))
# OBJ   = (SRC:src/%.c=build/%.o)                       # Substitution reference (VAR:pattern=replacement) SRC:src/%.c=build/%.o
TARGET  = $(OUTDIR)/sli                             # The final executable name
DEP     = $(OBJ:.o=.d)                                  # Creates .d-files

# --- Default rule (first one executed if you just type `make`) ---
all: $(TARGET)

# --- Link rule: link object files into the final program ---
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
# $@ = the target name (here: "program")
# $^ = all prerequisites (here: all .o files)
# This compiles the final binary from all object files.

# --- Compile rule: build object files from source ---
# Example: build/main.o depends on src/main.c
$(OUTDIR)/%.o: src/%.c | $(OUTDIR)
	$(CC) $(CFLAGS) -c $< -o $@
# $< = the first prerequisite (here: the matching .c file)
# $@ = the target (here: the .o file)
# The "| $(OUTDIR) means "make sure the build/ directory exists first"

# --- Directory rule: create build/ folder if missing ---
$(OUTDIR):
	mkdir -p $(OUTDIR)
# `mkdir -p` ensures the directory exists, without error if it already does

# --- Cleanup rule: remove generated files ---
clean:
	rm -rf $(OUTDIR) $(TARGET)
# -rf = recursive, force
# This deletes the build/ directory and the final program

# --- Dependencies ---
-include $(DEP)

.PHONY: all clean
