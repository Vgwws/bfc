/* This source code is licensed under MIT License */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bfc_asm.h"

static inline char* realloc_new_assembly(
		char* assembly, unsigned int* assembly_size, const char* template_assembly){
	if(strlen(template_assembly) + strlen(assembly) < *assembly_size){
		return assembly;
	}
	*assembly_size += strlen(template_assembly);
	char* new_assembly = realloc(assembly, *assembly_size * sizeof(char));
	if(!new_assembly){
		fprintf(stderr, "ERROR: Memory allocation for assembly failed\n");
		free(assembly);
		return NULL;
	}
	return new_assembly;
}

char* generate_asm_aarch64(
		AST* ast, unsigned int* assembly_size, unsigned int* depth){
	if(!ast) return NULL;
	const char* start_program =
		".global _start\n"
		".section .bss\n"
		"arr: .skip 30000\n"
		".section .text\n"
		"_start:\n"
		"  adr x3, arr\n"
		"  mov x4, 0\n"
		;
	const char* end_program =
		"  mov x0, 0\n"
		"  mov x8, 93\n"
		"  svc 0\n"
		;
	const char* val_inc =
		"  ldrb w1, [x3, x4]\n"
		"  add w1, w1, %d\n"
		"  strb w1, [x3, x4]\n"
		;
	const char* val_dec =
		"  ldrb w1, [x3, x4]\n"
		"  sub w1, w1, %d\n"
		"  strb w1, [x3, x4]\n"
		;
	const char* ptr_inc =
		"  add x4, x4, %d\n"
		;
	const char* ptr_dec =
		"  sub x4, x4, %d\n"
		;
	const char* start_loop =
		"loop%d_start:\n"
		"  ldrb w1, [x3, x4]\n"
		"  cmp w1, 0\n"
		"  beq loop%d_end\n"
		;
	const char* end_loop =
		"  b loop%d_start\n"
		"loop%d_end:\n"
		;
	const char* input =
		"  mov x0, 0\n"
		"  add x1, x3, x4\n"
		"  mov x2, 1\n"
		"  mov x8, 63\n"
		"  svc 0\n"
		;
	const char* output =
		"  mov x0, 1\n"
		"  add x1, x3, x4\n"
		"  mov x2, 1\n"
		"  mov x8, 64\n"
		"  svc 0\n"
		;
	char* assembly = malloc(*assembly_size * sizeof(char));
	if(!assembly){
		fprintf(stderr, "ERROR: Memory allocation for assembly failed\n");
		return NULL;
	}
	assembly[0] = '\0';
	unsigned int indew = 0;
	switch(ast->node.type){
		case AST_PROGRAM:
			assembly = realloc_new_assembly(assembly, assembly_size, start_program);
			if(!assembly) return NULL;
			indew += snprintf(assembly + indew, *assembly_size, "%s", start_program);
			for(unsigned int i = 0; i < ast->child_count; i++){
				char* new_assembly = generate_asm_aarch64(
						ast->child_nodes[i], assembly_size, depth);
				assembly = realloc_new_assembly(assembly, assembly_size, new_assembly);
				if(!assembly) return NULL;
				indew += snprintf(assembly + indew, *assembly_size, "%s", new_assembly);
			}
			assembly = realloc_new_assembly(assembly, assembly_size, end_program);
			if(!assembly) return NULL;
			snprintf(assembly + indew, *assembly_size, "%s", end_program);
			break;
		case AST_LOOP:
			*depth += 1;
			unsigned int preserve_depth = *depth;
			*depth += 1;
			assembly = realloc_new_assembly(assembly, assembly_size, start_loop);
			if(!assembly) return NULL;
			indew += snprintf(assembly + indew, *assembly_size,
					"loop%d_start:\n"
					"  ldrb w1, [x3, x4]\n"
					"  cmp w1, 0\n"
					"  beq loop%d_end\n",
					preserve_depth, preserve_depth);
			for(unsigned int i = 0; i < ast->child_count; i++){
				char* new_assembly = generate_asm_aarch64(
						ast->child_nodes[i], assembly_size, depth);
				assembly = realloc_new_assembly(assembly, assembly_size, new_assembly);
				if(!assembly) return NULL;
				indew += snprintf(assembly + indew, *assembly_size, "%s", new_assembly);
			}
			assembly = realloc_new_assembly(assembly, assembly_size, end_loop);
			if(!assembly) return NULL;
			snprintf(assembly + indew, *assembly_size,
					"  b loop%d_start\n"
					"loop%d_end:\n",
					preserve_depth, preserve_depth);
			break;
		case AST_VAL_INC:
			assembly = realloc_new_assembly(assembly, assembly_size, val_inc);
			if(!assembly) return NULL;
			snprintf(assembly, *assembly_size,
					"  ldrb w1, [x3, x4]\n"
					"  add w1, w1, %d\n"
					"  strb w1, [x3, x4]\n",
					ast->node.count);
			break;
		case AST_VAL_DEC:
			assembly = realloc_new_assembly(assembly, assembly_size, val_dec);
			if(!assembly) return NULL;
			snprintf(assembly, *assembly_size,
					"  ldrb w1, [x3, x4]\n"
					"  sub w1, w1, %d\n"
					"  strb w1, [x3, x4]\n",
					ast->node.count);
			break;
		case AST_PTR_INC:
			assembly = realloc_new_assembly(assembly, assembly_size, ptr_inc);
			if(!assembly) return NULL;
			snprintf(assembly, *assembly_size,
					"  add x4, x4, %d\n",
					ast->node.count);
			break;
		case AST_PTR_DEC:
			assembly = realloc_new_assembly(assembly, assembly_size, ptr_dec);
			if(!assembly) return NULL;
			snprintf(assembly, *assembly_size,
					"  sub x4, x4, %d\n",
					ast->node.count);
			break;
		case AST_INPUT:
			assembly = realloc_new_assembly(assembly, assembly_size, input);
			if(!assembly) return NULL;
			snprintf(assembly, *assembly_size, "%s", input);
			break;
		case AST_OUTPUT:
			assembly = realloc_new_assembly(assembly, assembly_size, output);
			if(!assembly) return NULL;
			snprintf(assembly, *assembly_size, "%s", output);
			break;
	}
	return assembly;
}
