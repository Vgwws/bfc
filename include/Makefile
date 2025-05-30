.PHONY: install uninstall

PREFIX ?= /usr/local
INCLUDE_DIR := $(PREFIX)/include

HEADERS := $(wildcard *.h)
INSTALL_HEADERS := $(HEADERS:%.h=$(INCLUDE_DIR)/%.h)

install: $(INSTALL_HEADERS)

$(INCLUDE_DIR)/%.h: %.h
	chmod 644 $<
	cp $< $@

uninstall:
	rm -f $(INSTALL_HEADERS)
