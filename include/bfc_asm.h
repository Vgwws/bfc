#ifndef BFC_ASM_H
#define BFC_ASM_H

#include "bfc_parser.h"

#define ASM_SIZE 1000

typedef enum {
	aarch64,
	x86_64,
	i386
} Arch;

typedef struct {
	FILE* output;
	ASTNodeType node_type;
	Arch arch;
} Context;

void generate_asm(AST* ast, Context context);

#endif
