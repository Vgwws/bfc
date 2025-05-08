.PHONY: all std debug test-exec test-asm test-all clean install uninstall

all: std debug

ARCH              ?= $(shell uname -m)

CC                ?= gcc

NAME              := bfc

CFLAGS            := -I include -Wall -Wextra -Werror -std=gnu99

C_SRC             := $(wildcard src/*.c)
STD_OBJ           := $(C_SRC:src/%.c=%.o)
DEBUG_OBJ         := $(C_SRC:src/%.c=%-debug.o)
BF_SRC            := $(wildcard test/*.bf)
EXEC              := $(BF_SRC:test/%.bf=%)
ASM               := $(BF_SRC:test/%.bf=%.s)
TEST_INPUT        ?= Hello
COMPILED_VARIANTS := $(wildcard $(NAME)*)

PREFIX            ?= /usr/local

BINDIR            := $(PREFIX)/bin
INCLUDEDIR        := $(PREFIX)/include

INSTALL_HEADERS   := $(wildcard include/*.h)
INSTALL_TARGETS   := $(INSTALL_HEADERS:include/%.h=$(INCLUDEDIR)/%.h) $(BINDIR)/$(NAME)

%: test/%.bf
	@echo "Test $@.bf Executable"
	@echo "-------------------------------------"
	./$(NAME)-debug -c $(CC) -t $(ARCH) -o $@ $<
	echo $(TEST_INPUT) | $(EMULATOR) ./$@
	@echo ""
	rm $@
	@echo "-------------------------------------\n"

%.s: test/%.bf
	@echo "Test $(notdir $<) Assembly"
	@echo "-------------------------------------"
	./$(NAME)-debug -c $(CC) -t $(ARCH) -S $@ $<
	cat $@
	rm $@
	@echo "-------------------------------------\n"

%.o: src/%.c
	gcc -c $< -o $@ $(CFLAGS) -O2

%-debug.o: src/%.c
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
	rm -f $(STD_OBJ)
	@strip $(NAME)

$(NAME)-debug: $(DEBUG_OBJ)
	gcc -g $(DEBUG_OBJ) -o $(NAME)-debug
	rm -f $(DEBUG_OBJ)

std: $(NAME)

debug: $(NAME)-debug

test-exec: $(NAME)-debug $(EXEC)

test-asm: $(NAME)-debug $(ASM)

test-all: $(NAME)-debug $(ASM) $(EXEC)

clean:
	rm -f $(ASM)
	rm -f $(EXEC)
	rm -f $(COMPILED_VARIANTS)

install: $(INSTALL_TARGETS)

uninstall:
	rm -f $(INSTALL_TARGETS)
