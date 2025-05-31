.PHONY: all asan-debug clean

NAME := bfc
PROGRAM_STD := ../$(NAME)
PROGRAM_DEBUG := ../$(NAME)-debug
PROGRAM_ASAN_DEBUG := ../$(NAME)-asan-debug

CFLAGS := -I ../include -Wall -Wextra -Werror -std=gnu99

C_SRC := $(wildcard *.c)
STD_OBJ := $(C_SRC:%.c=../build/%.o)
DEBUG_OBJ := $(C_SRC:%.c=../build/%-debug.o)
ASAN_DEBUG_OBJ := $(C_SRC:%.c=../build/%-asan-debug.o)

all: $(PROGRAM_STD) $(PROGRAM_DEBUG)

asan-debug: $(PROGRAM_ASAN_DEBUG)

clean:
	rm -f $(PROGRAM_STD)
	rm -f $(PROGRAM_DEBUG)
	rm -f $(PROGRAM_ASAN_DEBUG)
	rm -f $(STD_OBJ)
	rm -f $(DEBUG_OBJ)
	rm -f $(ASAN_DEBUG_OBJ)

../build/%.o: %.c
	@mkdir -p ../build
	gcc -c $(CFLAGS) -O2 $< -o $@

../build/%-debug.o: %.c
	@mkdir -p ../build
	gcc -c -g $(CFLAGS) -O0 $< -o $@

../build/%-asan-debug.o: %.c
	@mkdir -p ../build
	clang -fsanitize=address -c -g $(CFLAGS) -O0 $< -o $@

$(PROGRAM_STD): $(STD_OBJ)
	gcc $^ -o $@
	@strip $@

$(PROGRAM_DEBUG): $(DEBUG_OBJ)
	gcc -g $^ -o $@

$(PROGRAM_ASAN_DEBUG): $(ASAN_DEBUG_OBJ)
	clang -fsanitize=address -g $^ -o $@
