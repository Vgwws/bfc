#ifndef BFC_ASM_H
#define BFC_ASM_H

#include "bfc_parser.h"

#define ASM_SIZE 1000

typedef enum {
	aarch64,
	x86_64
} Arch;

char* generate_asm(
		AST* ast, unsigned int* assembly_size, unsigned int* depth, Arch arch);

#endif
