.PHONY: all std debug test asan-debug object-compile clean install uninstall

CFLAGS := -I include -Wall -Wextra -Werror -std=c99

C_SRC := $(wildcard src/*.c)
STD_OBJ := $(C_SRC:src/%.c=build/std/%.o)
DEBUG_OBJ := $(C_SRC:src/%.c=build/debug/%.o)
ASAN_OBJ := $(C_SRC:src/%.c=build/asan-debug/%.o)
BF_SRC := $(wildcard test/*.bf)
ASM := $(BF_SRC:test/%.bf=%.s)
OBJ := $(ASM:%.s=%.o)
EXEC := $(OBJ:%.o=%)

PREFIX ?= /usr/local

BINDIR := $(PREFIX)/bin
INCLUDEDIR := $(PREFIX)/include

%.s: test/%.bf ebfc
	./ebfc -o $@ $<

%.o: %.s
	as -o $@ $<
	rm $<

%: %.o
	ld -o $@ $<
	@echo "Output of $@:"
	@./$@
	@echo "\n-------"
	rm $<
	rm $@

build/std/%.o: src/%.c
	mkdir -p $(dir $@)
	gcc -c $< -o $@ $(CFLAGS) -O2

build/debug/%.o: src/%.c
	mkdir -p $(dir $@)
	gcc -c $< -o $@ $(CFLAGS) -O0

build/asan-debug/%.o: src/%.c
	mkdir -p $(dir $@)
	clang -fsanitize=address -c $< -o $@ $(CFLAGS) -O0

all: std debug test

std: $(STD_OBJ)
	gcc $(STD_OBJ) -o ebfc

debug: $(DEBUG_OBJ)
	gcc $(DEBUG_OBJ) -o ebfc-debug

asan-debug: $(ASAN_OBJ)
	clang -fsanitize=address $(ASAN_OBJ) -o ebfc-asan-debug

test: std $(EXEC)

clean:
	rm -f $(STD_OBJ)
	rm -f $(DEBUG_OBJ)
	rm -f $(ASAN_OBJ)
	rm -f ebfc*

install:
	mkdir -p $(INCLUDEDIR)
	mkdir -p $(BINDIR)
	chmod 755 ebfc
	chmod 644 include/*.h
	cp ebfc $(BINDIR)
	cp include/* $(INCLUDEDIR)
	@echo "Installation done"

uninstall:
	rm -f $(BINDIR)/ebfc
	rm -f $(INCLUDEDIR)/ebfc*.h
