.PHONY: all asan-debug test-exec test-asm test-all clean install uninstall

ARCH              ?= $(shell uname -m)

CC                ?= gcc

TEST_INPUT        ?= Hello

NAME              := bfc

PREFIX            ?= /usr/local

BINDIR            := $(PREFIX)/bin

INSTALL_TARGETS   := $(BINDIR)/$(NAME)

ifeq ($(STATIC),no)
	STATIC_FLAG :=
else
	STATIC_FLAG := -static
endif

all:
	@$(MAKE) -C src STATIC=$(STATIC_FLAG) all

asan-debug:
	@$(MAKE) -C src asan-debug

$(BINDIR)/$(NAME): $(NAME)
	@mkdir -p $(dir $@)
	cp $< $@
	chmod 755 $@

test-exec: all
	@$(MAKE) -C test NAME=../$(NAME) ARCH=$(ARCH) CC=$(CC) TEST_INPUT="$(TEST_INPUT)" EMULATOR="$(EMULATOR)" test-exec

test-asm: all
	@$(MAKE) -C test NAME=../$(NAME) ARCH=$(ARCH) CC=$(CC) test-asm

test-all: all
	@$(MAKE) -C test NAME=../$(NAME) ARCH=$(ARCH) CC=$(CC) TEST_INPUT="$(TEST_INPUT)" EMULATOR="$(EMULATOR)" test-all

clean:
	@$(MAKE) -C src clean

install: $(INSTALL_TARGETS)
	@$(MAKE) -C include install

uninstall:
	@$(MAKE) -C include uninstall
	rm -f $(INSTALL_TARGETS)
