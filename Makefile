.PHONY: all std debug test asan-debug object-compile clean install uninstall

CFLAGS := -I include -Wall -Wextra -Werror -std=c99

C_SRC := $(wildcard src/*.c)
STD_OBJ := $(C_SRC:src/%.c=build/std/%.o)
DEBUG_OBJ := $(C_SRC:src/%.c=build/debug/%.o)
ASAN_OBJ := $(C_SRC:src/%.c=build/asan-debug/%.o)
BF_SRC := $(wildcard test/*.bf)
EXEC := $(BF_SRC:test/%.bf=%)

PREFIX ?= /usr/local

BINDIR := $(PREFIX)/bin
INCLUDEDIR := $(PREFIX)/include

%: test/%.bf
	./bfc -o $@ $<
	@echo "Output of $@:"
	@./$@
	@echo "\n-----------"
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

all: std debug

std: $(STD_OBJ)
	gcc $(STD_OBJ) -o bfc

debug: $(DEBUG_OBJ)
	gcc $(DEBUG_OBJ) -o bfc-debug

asan-debug: $(ASAN_OBJ)
	clang -fsanitize=address $(ASAN_OBJ) -o bfc-asan-debug

test: std $(EXEC)

clean:
	rm -f $(STD_OBJ)
	rm -f $(DEBUG_OBJ)
	rm -f $(ASAN_OBJ)
	rm -f bfc*

install:
	mkdir -p $(INCLUDEDIR)
	mkdir -p $(BINDIR)
	chmod 755 bfc
	chmod 644 include/*.h
	cp bfc $(BINDIR)
	cp include/* $(INCLUDEDIR)
	@echo "Installation done"

uninstall:
	rm -f $(BINDIR)/bfc
	rm -f $(INCLUDEDIR)/bfc*.h
