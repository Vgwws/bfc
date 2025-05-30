.PHONY: test-all test-exec test-asm

BF_SRC     := $(wildcard *.bf)
ASM        := $(BF_SRC:%.bf=%.s)
EXEC       := $(BF_SRC:%.bf=%)

TEST_INPUT ?= Hello

%: %.bf
	@echo "Test $< Executable"
	@echo "-------------------------------------"
	cat $<
	./$(NAME)-$(PREFIX_DEBUG)-debug -c "$(CC)" -t $(ARCH) -o $@ $<
	echo "$(TEST_INPUT)" | $(EMULATOR) ./$@
	@echo ""
	rm $@
	@echo "-------------------------------------\n"

%.s: %.bf
	@echo "Test $< Assembly"
	@echo "-------------------------------------"
	cat $<
	./$(NAME)-$(PREFIX_DEBUG)-debug -t $(ARCH) -S $@ $<
	cat $@
	rm $@
	@echo "-------------------------------------\n"

test-all: test-asm test-exec

test-exec: $(EXEC)

test-asm: $(ASM)
