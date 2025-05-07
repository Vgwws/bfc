.PHONY: all std debug test-exec test-asm clean install uninstall

all: std debug

NAME := bfc

CFLAGS := -I include -Wall -Wextra -Werror -std=gnu99

C_SRC := $(wildcard src/*.c)
STD_OBJ := $(C_SRC:src/%.c=build/std/%.o)
DEBUG_OBJ := $(C_SRC:src/%.c=build/debug/%.o)
BF_SRC := $(wildcard test/*.bf)
EXEC := $(BF_SRC:test/%.bf=%)
ASM := $(BF_SRC:test/%.bf=%.s)
COMPILED_VARIANTS := $(wildcard $(NAME)*)

PREFIX ?= /usr/local

BINDIR := $(PREFIX)/bin
INCLUDEDIR := $(PREFIX)/include

INSTALL_HEADERS := $(wildcard include/*.h)
INSTALL_TARGETS := $(INSTALL_HEADERS:include/%.h=$(INCLUDEDIR)/%.h) $(BINDIR)/$(NAME)

%: test/%.bf
	@echo "Source code of $@:"
	@cat $<
	./$(NAME)-debug -o $@ $<
	@echo "Output of $@:"
	@./$@
	@echo "\n-----------"
	rm $@

%.s: test/%.bf
	@echo "Source code of $@:"
	@cat $<
	./$(NAME)-debug -S $@ $<
	@echo "Assembly of $@:"
	@cat $@
	@echo "\n-------"
	rm $@

build/std/%.o: src/%.c
	@mkdir -p $(dir $@)
	gcc -c $< -o $@ $(CFLAGS) -O2

build/debug/%.o: src/%.c
	@mkdir -p $(dir $@)
	gcc -g -c $< -o $@ $(CFLAGS) -O0

$(INCLUDEDIR)/%.h: include/%.h
	@mkdir -p $(dir $@)
	cp $< $@
	chmod 644 $@

$(BINDIR)/$(NAME): $(NAME)
	@mkdir -p $(dir $@)
	cp $< $@
	chmod 755 $@

$(NAME): $(STD_OBJ)
	gcc $(STD_OBJ) -o $(NAME)
	@strip $(NAME)

$(NAME)-debug: $(DEBUG_OBJ)
	gcc -g $(DEBUG_OBJ) -o $(NAME)-debug

std: $(NAME)

debug: $(NAME)-debug

test-exec: $(NAME)-debug $(EXEC)

test-asm: $(NAME)-debug $(ASM)

clean:
	rm -f $(STD_OBJ)
	rm -f $(DEBUG_OBJ)
	rm -f $(ASAN_OBJ)
	rm -f $(COMPILED_VARIANTS)

install: $(INSTALL_TARGETS)

uninstall:
	rm -f $(INSTALL_TARGETS)
