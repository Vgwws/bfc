# Brainfuck Compiler
## Short Description
A Brainfuck Compiler that compiling Brainfuck into Machine Code, written in pure C
## Supported CPU Architecture
Supported CPU Architecture:
- AArch64
## How to build it
First install necessary dependencies
```sh
# ArchLinux
sudo pacman -S git gcc make binutils
# Fedora
sudo dnf install git gcc make binutils
# Debian-based
sudo apt install git gcc make binutils
```
Clone the repository
```sh
git clone https://github.com/Vgwws/bfc.git
cd bfc
```
Build it
```sh
# Standard version
make std
# Debug version
make debug
# ASan Debug version(Install Clang first)
make asan-debug
# Both version
make all
```
If you want to clean the compiled files
```sh
make clean
```
Install it
```sh
make install
```
If you want to uninstall the installed files
```sh
make uninstall
```
