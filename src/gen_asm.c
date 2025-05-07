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
	char* new_assembly = realloc(assembly, *assembly_size);
	if(!new_assembly){
		fprintf(stderr, "ERROR: Memory allocation for assembly failed\n");
		free(assembly);
		return NULL;
	}
	return new_assembly;
}

char* generate_asm(
		AST* ast, unsigned int* assembly_size, unsigned int* depth){
	if(!ast) return NULL;
#if defined(__aarch64__)
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
#elif defined(__x86_64__)
	const char* start_program =
		".global _start\n"
		".section .bss\n"
		"arr: .skip 30000\n"
		".section .text\n"
		"_start:\n"
		"  lea arr(%rip), %r10\n"
		"  xor %r11, %r11\n"
		;
	const char* end_program =
		"  mov $60, %rax\n"
		"  mov $0, %rbx\n"
		"  syscall\n"
		;
	const char* ptr_inc =
		"  addl $%d, %%r11\n"
		;
	const char* ptr_dec =
		"  subl $%d, %%r11\n"
		;
	const char* val_inc =
		"  mov (%%r10, %%r11), %%r9\n"
		"  add $%d, %%r9\n"
		"  mov %%r9, (%%r10, %%r11)\n"
		;
	const char* val_dec =
		"  mov (%%r10, %%r11), %%r9\n"
		"  sub $%d, %%r9\n"
		"  mov %%r9, (%%r10, %%r11)\n"
		;
	const char* start_loop =
		"loop%d_start:\n"
		"  mov (%%r10, %%r11), %%r9\n"
		"  test %%r9, %%r9\n"
		"  jz loop%d_end\n"
		;
	const char* end_loop =
		"  jmp loop%d_start\n"
		"loop%d_end:\n"
		;
	const char* input =
		"  mov $0, %rax\n"
		"  mov $0, %rdi\n"
		"  mov (%r10, %r11), %rsi\n"
		"  mov $1, %rdx\n"
		"  syscall\n"
		;
	const char* output =
		"  mov $1, %rax\n"
		"  mov $1, %rdi\n"
		"  mov (%r10, %r11), %rsi\n"
		"  mov $1, %rdx\n"
		"  syscall\n"
		;
#endif
	char* assembly = malloc(*assembly_size);
	if(!assembly){
		fprintf(stderr, "ERROR: Memory allocation for assembly failed\n");
		return NULL;
	}
	assembly[0] = '\0';
	unsigned int index = 0;
	switch(ast->node.type){
		case AST_PROGRAM:
			assembly = realloc_new_assembly(assembly, assembly_size, start_program);
			if(!assembly) return NULL;
			index += snprintf(assembly + index, *assembly_size, "%s", start_program);
			for(unsigned int i = 0; i < ast->child_count; i++){
				char* new_assembly = generate_asm(
						ast->child_nodes[i], assembly_size, depth);
				assembly = realloc_new_assembly(assembly, assembly_size, new_assembly);
				if(!assembly) return NULL;
				index += snprintf(assembly + index, *assembly_size, "%s", new_assembly);
				free(new_assembly);
			}
			assembly = realloc_new_assembly(assembly, assembly_size, end_program);
			if(!assembly) return NULL;
			snprintf(assembly + index, *assembly_size, "%s", end_program);
			break;
		case AST_LOOP:
			*depth += 1;
			unsigned int preserve_depth = *depth;
			assembly = realloc_new_assembly(assembly, assembly_size, start_loop);
			if(!assembly) return NULL;
#if defined(__aarch64)
			index += snprintf(assembly + index, *assembly_size,
					"loop%d_start:\n"
					"  ldrb w1, [x3, x4]\n"
					"  cbz w1, loop%d_end\n",
					preserve_depth, preserve_depth);
#elif defined(__x86_64__)
			index += snprintf(assembly + index, *assembly_size,
					"loop%d_start:\n"
					"  mov (%%r10, %%r11), %%r9\n"
					"  test %%r9, %%r9\n"
					"  jz loop%d_end\n",
					preserve_depth, preserve_depth);
#endif
			for(unsigned int i = 0; i < ast->child_count; i++){
				char* new_assembly = generate_asm(
						ast->child_nodes[i], assembly_size, depth);
				assembly = realloc_new_assembly(assembly, assembly_size, new_assembly);
				if(!assembly) return NULL;
				index += snprintf(assembly + index, *assembly_size, "%s", new_assembly);
				free(new_assembly);
			}
			assembly = realloc_new_assembly(assembly, assembly_size, end_loop);
			if(!assembly) return NULL;
#if defined(__aarch64__)
			snprintf(assembly + index, *assembly_size,
					"  b loop%d_start\n"
					"loop%d_end:\n",
					preserve_depth, preserve_depth);
#elif defined(__x86_64__)
			snprintf(assembly + index, *assembly_size,
					"  jmp loop%d_start\n"
					"loop%d_end:\n",
					preserve_depth, preserve_depth);
#endif
			break;
		case AST_VAL_INC:
			assembly = realloc_new_assembly(assembly, assembly_size, val_inc);
			if(!assembly) return NULL;
#if defined(__aarch64__)
			snprintf(assembly, *assembly_size,
					"  ldrb w1, [x3, x4]\n"
					"  add w1, w1, %d\n"
					"  strb w1, [x3, x4]\n",
					ast->node.count);
#elif defined(__x86_64)
			snprintf(assembly, *assembly_size,
					"  mov (%%r10, %%r11), %%r9\n"
					"  add $%d, %%r9\n"
					"  mov %%r9, (%%r10, %%r11)\n",
					ast->node.count);
#endif
			break;
		case AST_VAL_DEC:
			assembly = realloc_new_assembly(assembly, assembly_size, val_dec);
			if(!assembly) return NULL;
#if defined(__aarch64__)
			snprintf(assembly, *assembly_size,
					"  ldrb w1, [x3, x4]\n"
					"  sub w1, w1, %d\n"
					"  strb w1, [x3, x4]\n",
					ast->node.count);
#elif defined(__x86_64__)
			snprintf(assembly, *assembly_size,
					"  mov (%%r10, %%r11), %%r9\n"
					"  sub $%d, %%r9\n"
					"  mov %%r9, (%%r10, %%r11)\n",
					ast->node.count);
#endif
			break;
		case AST_PTR_INC:
			assembly = realloc_new_assembly(assembly, assembly_size, ptr_inc);
			if(!assembly) return NULL;
#if defined(__aarch64__)
			snprintf(assembly, *assembly_size,
					"  add x4, x4, %d\n",
					ast->node.count);
#elif defined(__x86_64__)
			snprintf(assembly, *assembly_size,
					"  add $%d, %%r11\n",
					ast->node.count);
#endif
			break;
		case AST_PTR_DEC:
			assembly = realloc_new_assembly(assembly, assembly_size, ptr_dec);
			if(!assembly) return NULL;
#if defined(__aarch64__)
			snprintf(assembly, *assembly_size,
					"  sub x4, x4, %d\n",
					ast->node.count);
#elif defined(__x86_64__)
			snprintf(assembly, *assembly_size,
					"  sub $%d, %%r11\n",
					ast->node.count);
#endif
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
