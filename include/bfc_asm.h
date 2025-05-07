#ifndef BFC_ASM_H
#define BFC_ASM_H

#include "bfc_parser.h"

#define ASM_SIZE 1000

char* generate_asm(
		AST* ast, unsigned int* assembly_size, unsigned int* depth);

#endif
