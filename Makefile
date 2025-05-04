.PHONY: all std debug object-compile clean install uninstall

all: std debug

std: src/*.c
	mkdir -p build/
	gcc -c src/*.c -O0
	mv *.o build/
	gcc build/*.o -I include -Wall -Wextra -Werror -std=c99 -o ebfc

debug: src/*.c
	mkdir -p build/
	gcc -g -c src/*.c -Og
	mv *.o build/
	gcc build/main.o build/lexer.o build/parser.o -I include -Wall -Wextra -Werror -std=c99 -o ebfc-debug

clean:
	rm -f build/*
	rm -f ebfc*

PREFIX ?= /usr/local

INCLUDEDIR = $(PREFIX)/include

BINDIR = $(PREFIX)/bin

install:
	mkdir -p $(INCLUDEDIR)
	mkdir -p $(BINDIR)
	chmod 755 ebfc
	chmod 644 include/*
	cp ebfc $(BINDIR)
	cp include/* $(INCLUDEDIR)
	@echo "BINDIR = $(BINDIR)"
	@echo "INCLUDEDIR = $(INCLUDEDIR)"

uninstall:
	rm -f $(BINDIR)/ebfc
	rm -f $(INCLUDEDIR)/ebfc*
