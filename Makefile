.PHONY: all std debug asan-debug object-compile clean install uninstall

all: std debug

std: src/*.c
	mkdir -p build/std/
	gcc -c src/*.c -I include -Wall -Wextra -Werror -std=c99 -O0
	mv *.o build/std/
	gcc build/std/*.o -o ebfc

debug: src/*.c
	mkdir -p build/debug/
	gcc -g -c src/*.c -I include -Wall -Wextra -Werror -std=c99 -Og
	mv *.o build/debug/
	gcc build/debug/*.o -I include -Wall -Wextra -Werror -std=c99 -o ebfc-debug

asan-debug: src/*.c
	mkdir -p build/debug/
	clang -fsanitize=address -g -c src/*.c -I include -Wall -Wextra -Werror -std=c99 -Og
	mv *.o build/debug/
	clang -fsanitize=address build/debug/*.o -I include -Wall -Wextra -Werror -std=c99 -o ebfc-asan-debug

clean:
	rm -f build/std/*.o
	rm -f build/debug/*.o
	rm -f ebfc*

PREFIX ?= /usr/local

INCLUDEDIR = $(PREFIX)/include

BINDIR = $(PREFIX)/bin

install:
	mkdir -p $(INCLUDEDIR)
	mkdir -p $(BINDIR)
	chmod 755 ebfc
	chmod 644 include/*.h
	cp ebfc $(BINDIR)
	cp include/* $(INCLUDEDIR)
	@echo "BINDIR = $(BINDIR)"
	@echo "INCLUDEDIR = $(INCLUDEDIR)"

uninstall:
	rm -f $(BINDIR)/ebfc
	rm -f $(INCLUDEDIR)/ebfc*.h
