# Brainfuck Compiler
## Short Description
A Brainfuck Compiler that compiling Brainfuck into Machine Code or Assembly, written in pure C
## Status
Still non production ready
## Supported CPU Architecture and Platform
Supported CPU Architecture:
- AArch64 Linux
- x86_64 Linux
## How to install it
First install necessary dependencies
### ArchLinux
```sh
sudo pacman -S git gcc make
```
### Fedora
```sh
sudo dnf install git gcc make
```
### Debian-based
```sh
sudo apt install git gcc make
```
Clone the repository
```sh
git clone https://github.com/Vgwws/bfc.git
cd bfc
```
Install it:
```sh
make install
```
If you want to uninstall it
```sh
make uninstall
```
## Options:
\-o [FILENAME] : Set output files(Machine Code) to [FILENAME]

\-S [FILENAME] : Set output files(Assembly) to [FILENAME]

\-t [ARCH]     : Set architecture target into [ARCH]
                (e.g., AArch64, x86_64)

\-c [PROGRAM]  : Set assembler and linker of Assembly to [PROGRAM]

                (e.g., gcc, clang)

                Potentially be deprecated after new independent assembler and linker added

\-h            : Print this help
