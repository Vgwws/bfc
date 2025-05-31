# MAKEFILE
## PHONY Target
- `all`: Build both debug and non-debug version of the program

- `debug`: Build only debug version of the program

- `asan-debug`: Build ASan debug version(Need clang)

- `clean`: Remove all Object files, And all versions of the program

- `install`: Install Headers and non-debug version of the program

- `uninstall`: Uninstall Headers and non-debug version of the program

- `test-asm`: Test program to generate Assembly

- `test-exec`: Test program to generate executable and then execute it

- `test-all`: Test both

## Flags
- `ARCH`: Set architecture that the program will generate

- `CC`: Set Assembler and linker when testing program

- `EMULATOR`: Set emulator when testing program if the program are cross compiling

- `TEST_INPUT`: Set test input when testing program

## Supported Architecture
|Architecture|Status      |
|------------|------------|
|x86_64      |Full support|
|i386        |Full support|
|aarch64     |Full support|
|aarch32     |Full support|
