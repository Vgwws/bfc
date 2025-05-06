.PHONY: all std debug test asan-debug object-compile clean install uninstall

CFLAGS := -I include -Wall -Wextra -Werror -std=gnu99

C_SRC := $(wildcard src/*.c)
STD_OBJ := $(C_SRC:src/%.c=build/std/%.o)
DEBUG_OBJ := $(C_SRC:src/%.c=build/debug/%.o)
ASAN_OBJ := $(C_SRC:src/%.c=build/asan-debug/%.o)
BF_SRC := $(wildcard test/*.bf)
EXEC := $(BF_SRC:test/%.bf=%)
BFC_VARIANTS := $(wildcard bfc*)

PREFIX ?= /usr/local

BINDIR := $(PREFIX)/bin
INCLUDEDIR := $(PREFIX)/include

INSTALL_HEADERS := $(wildcard include/*.h)
INSTALL_TARGETS := $(patsubst include/%, $(INCLUDEDIR)/%, $(INSTALL_HEADERS)) $(BINDIR)/bfc

%: test/%.bf
	./bfc-debug -o $@ $<
	@echo "Output of $@:"
	@./$@
	@echo "\n-----------"
	rm $@

build/std/%.o: src/%.c
	mkdir -p $(dir $@)
	gcc -c $< -o $@ $(CFLAGS) -O2

build/debug/%.o: src/%.c
	mkdir -p $(dir $@)
	gcc -g -c $< -o $@ $(CFLAGS) -O0

build/asan-debug/%.o: src/%.c
	mkdir -p $(dir $@)
	clang -fsanitize=address -g -c $< -o $@ $(CFLAGS) -O0

$(INCLUDEDIR)/%.h: include/%.h
	@mkdir -p $(dir $@)
	cp $< $@
	chmod 644 $@

$(BINDIR)/bfc: bfc
	@mkdir -p $(dir $@)
	cp $< $@
	chmod 755 $@

all: std debug

std: $(STD_OBJ)
	gcc $(STD_OBJ) -o bfc

debug: $(DEBUG_OBJ)
	gcc -g $(DEBUG_OBJ) -o bfc-debug

asan-debug: $(ASAN_OBJ)
	clang -fsanitize=address -g $(ASAN_OBJ) -o bfc-asan-debug

test: debug $(EXEC)

clean:
	rm -f $(STD_OBJ)
	rm -f $(DEBUG_OBJ)
	rm -f $(ASAN_OBJ)
	rm -f $(BFC_VARIANTS)

install: uninstall $(INSTALL_TARGETS)

uninstall:
	rm -f $(BINDIR)/bfc
	rm -f $(patsubst include/%, $(INCLUDEDIR)/%, $(INSTALL_HEADERS))
