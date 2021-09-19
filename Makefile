# Makefile for alphac

CC=gcc
CFLAGS=-Wall -g

BUILD_DIR=bin
SOURCES=$(wildcard src/*.c src/memory/*.c src/ALU/*.c src/std_lib/*.c\
          src/libs/*.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))

ASC_FILES=$(wildcard test_files/Errors/*.asc test_files/Working/*.asc)
ABC_FILES=$(patsubst %.asc, %.abc, $(ASC_FILES))

EXECUTABLE=$(BUILD_DIR)/avm

.PHONY: objects clean all format compile $(ABC_FILES)

all: build $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@$(CC) $(CFLAGS) -o $@ $(OBJECTS) -lm
	@printf "\n-> avm binary successfully built in /bin\n\n"

build:
	@mkdir -p $(BUILD_DIR)

$(OBJECTS): src/%.o : src/%.c
	@$(CC) $(CFLAGS) -c $< -o $@


compile: $(BUILD_DIR)/parser $(ABC_FILES)

$(ABC_FILES): test_files/%.abc : test_files/%.asc
	./$(BUILD_DIR)/parser $< -o $@


# Valgrind memcheck test
# memcheck: avm
# 	@valgrind --leak-check=full \
# 	         --show-leak-kinds=definite,indirect,possible,reachable \
# 	         --track-origins=yes \
# 	         --verbose \
# 	         --log-file=memcheck.out \
# 	         $(BUILD_DIR)/avm
# 	@printf "\n-> Valgrind memcheck log file (memcheck.out) generated\n\n"


# Formatting
format:
	@find . -name "*.[ch]" | xargs clang-format -style=file -i


# Cleaning stuff
clean:
	@find . -name "*~" -exec rm {} \;
	@find . -name "*.o" -exec rm {} \;
	@rm -f memcheck.out
	@clear

distclean: clean
	@find . -name "*.abc" -exec rm {} \;
	@rm -f $(BUILD_DIR)/*
	@clear
