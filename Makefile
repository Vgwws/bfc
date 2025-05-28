.PHONY: all debug test-exec test-asm test-all clean install uninstall

ARCH              ?= $(shell uname -m)

CC                ?= gcc

NAME              := bfc

CFLAGS            := -I include -Wall -Wextra -Werror -std=gnu99

C_SRC             := $(wildcard src/*.c)
STD_OBJ           := $(C_SRC:src/%.c=build/%.o)
DEBUG_OBJ         := $(C_SRC:src/%.c=build/%-debug.o)
OBJ               := $(STD_OBJ) $(DEBUG_OBJ)
BF_SRC            := $(wildcard test/*.bf)
EXEC              := $(BF_SRC:test/%.bf=%)
ASM               := $(BF_SRC:test/%.bf=%.s)
TEST_INPUT        ?= Hello

PREFIX            ?= /usr/local

BINDIR            := $(PREFIX)/bin
INCLUDEDIR        := $(PREFIX)/include

INSTALL_HEADERS   := $(wildcard include/*.h)
INSTALL_TARGETS   := $(INSTALL_HEADERS:include/%.h=$(INCLUDEDIR)/%.h) $(BINDIR)/$(NAME)

all: $(NAME)

%: test/%.bf
	@echo "Test $@.bf Executable"
	@echo "-------------------------------------"
	./$(NAME)-debug -c "$(CC)" -t $(ARCH) -o $@ $<
	echo "$(TEST_INPUT)" | $(EMULATOR) ./$@
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

build/%.o: src/%.c
	@mkdir -p build
	gcc -c $< -o $@ $(CFLAGS) -O2

build/%-debug.o: src/%.c
	@mkdir -p build
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

debug: $(NAME)-debug

test-exec: $(NAME)-debug $(EXEC)

test-asm: $(NAME)-debug $(ASM)

test-all: $(NAME)-debug $(ASM) $(EXEC)

clean:
	rm -f $(OBJ)
	rm -f $(ASM)
	rm -f $(EXEC)
	rm -f $(NAME)
	rm -f $(NAME)-debug

install: $(INSTALL_TARGETS)

uninstall:
	rm -f $(INSTALL_TARGETS)
