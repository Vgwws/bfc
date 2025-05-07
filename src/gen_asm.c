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
		AST* ast, unsigned int* assembly_size, unsigned int* depth, Arch arch){
	if(!ast) return NULL;
	char* start_program = NULL;
	char* end_program = NULL;
	char* val_inc = NULL;
	char* val_dec = NULL;
	char* ptr_inc = NULL;
	char* ptr_dec = NULL;
	char* start_loop = NULL;
	char* end_loop = NULL;
	char* input = NULL;
	char* output = NULL;
	switch(arch){
		case aarch64:
			start_program =
				".global _start\n"
				".section .bss\n"
				"arr: .skip 30000\n"
				".section .text\n"
				"_start:\n"
				"  adr x3, arr\n"
				"  mov x4, 0\n"
				;
			end_program =
				"  mov x0, 0\n"
				"  mov x8, 93\n"
				"  svc 0\n"
				;
			val_inc =
				"  ldrb w1, [x3, x4]\n"
				"  add w1, w1, %d\n"
				"  strb w1, [x3, x4]\n"
				;
			val_dec =
				"  ldrb w1, [x3, x4]\n"
				"  sub w1, w1, %d\n"
				"  strb w1, [x3, x4]\n"
				;
			ptr_inc =
				"  add x4, x4, %d\n"
				;
			ptr_dec =
				"  sub x4, x4, %d\n"
				;
			start_loop =
				"loop%d_start:\n"
				"  ldrb w1, [x3, x4]\n"
				"  cmp w1, 0\n"
				"  beq loop%d_end\n"
				;
			end_loop =
				"  b loop%d_start\n"
				"loop%d_end:\n"
				;
			input =
				"  mov x0, 0\n"
				"  add x1, x3, x4\n"
				"  mov x2, 1\n"
				"  mov x8, 63\n"
				"  svc 0\n"
				;
			output =
				"  mov x0, 1\n"
				"  add x1, x3, x4\n"
				"  mov x2, 1\n"
				"  mov x8, 64\n"
				"  svc 0\n"
				;
			break;
		case x86_64:
			start_program =
				".global _start\n"
				".section .bss\n"
				"arr: .skip 30000\n"
				".section .text\n"
				"_start:\n"
				"  lea arr(%rip), %r10\n"
				"  xor %r11, %r11\n"
				;
			end_program =
				"  mov $60, %rax\n"
				"  mov $0, %rdi\n"
				"  syscall\n"
				;
			ptr_inc =
				"  add $%d, %%r11\n"
				;
			ptr_dec =
				"  sub $%d, %%r11\n"
				;
			val_inc =
				"  addb $%d, (%%r10, %%r11)\n"
				;
			val_dec =
				"  subb $%d, (%%r10, %%r11)\n"
				;
			start_loop =
				"loop%d_start:\n"
				"  movzbl (%%r10, %%r11), %%r9d\n"
				"  test %%r9d, %%r9d\n"
				"  jz loop%d_end\n"
				;
			end_loop =
				"  jmp loop%d_start\n"
				"loop%d_end:\n"
				;
			input =
				"  push %rcx\n"
				"  push %r11\n"
				"  mov $0, %rax\n"
				"  mov $0, %rdi\n"
				"  lea arr(%rip), %rsi\n"
				"  add %r11, %rsi\n"
				"  mov $1, %rdx\n"
				"  syscall\n"
				"  pop %r11\n"
				"  pop %rcx\n"
				;
			output =
				"  push %rcx\n"
				"  push %r11\n"
				"  mov $1, %rax\n"
				"  mov $1, %rdi\n"
				"  lea arr(%rip), %rsi\n"
				"  add %r11, %rsi\n"
				"  mov $1, %rdx\n"
				"  syscall\n"
				"  pop %r11\n"
				"  pop %rcx\n"
				;
			break;
	}
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
						ast->child_nodes[i], assembly_size, depth, arch);
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
			switch(arch){
				case aarch64:
					index += snprintf(assembly + index, *assembly_size,
							"loop%d_start:\n"
							"  ldrb w1, [x3, x4]\n"
							"  cbz w1, loop%d_end\n",
							preserve_depth, preserve_depth);
					break;
				case x86_64:
					index += snprintf(assembly + index, *assembly_size,
							"loop%d_start:\n"
							"  movzbl (%%r10, %%r11), %%r9d\n"
							"  test %%r9d, %%r9d\n"
							"  jz loop%d_end\n",
							preserve_depth, preserve_depth);
					break;
			}
			for(unsigned int i = 0; i < ast->child_count; i++){
				char* new_assembly = generate_asm(
						ast->child_nodes[i], assembly_size, depth, arch);
				assembly = realloc_new_assembly(assembly, assembly_size, new_assembly);
				if(!assembly) return NULL;
				index += snprintf(assembly + index, *assembly_size, "%s", new_assembly);
				free(new_assembly);
			}
			assembly = realloc_new_assembly(assembly, assembly_size, end_loop);
			if(!assembly) return NULL;
			switch(arch){
				case aarch64:
					snprintf(assembly + index, *assembly_size,
							"  b loop%d_start\n"
							"loop%d_end:\n",
							preserve_depth, preserve_depth);
					break;
				case x86_64:
					snprintf(assembly + index, *assembly_size,
							"  jmp loop%d_start\n"
							"loop%d_end:\n",
							preserve_depth, preserve_depth);
					break;
			}
			break;
		case AST_VAL_INC:
			assembly = realloc_new_assembly(assembly, assembly_size, val_inc);
			if(!assembly) return NULL;
			switch(arch){
				case aarch64:
					snprintf(assembly, *assembly_size,
							"  ldrb w1, [x3, x4]\n"
							"  add w1, w1, %d\n"
							"  strb w1, [x3, x4]\n",
							ast->node.count);
					break;
				case x86_64:
					snprintf(assembly, *assembly_size,
							"  addb $%d, (%%r10, %%r11)\n",
							ast->node.count);
					break;
			}
			break;
		case AST_VAL_DEC:
			assembly = realloc_new_assembly(assembly, assembly_size, val_dec);
			if(!assembly) return NULL;
			switch(arch){
				case aarch64:
					snprintf(assembly, *assembly_size,
							"  ldrb w1, [x3, x4]\n"
							"  sub w1, w1, %d\n"
							"  strb w1, [x3, x4]\n",
							ast->node.count);
					break;
				case x86_64:
					snprintf(assembly, *assembly_size,
							"  subb $%d, (%%r10, %%r11)\n",
							ast->node.count);
					break;
			}
			break;
		case AST_PTR_INC:
			assembly = realloc_new_assembly(assembly, assembly_size, ptr_inc);
			if(!assembly) return NULL;
			switch(arch){
				case aarch64:
					snprintf(assembly, *assembly_size,
							"  add x4, x4, %d\n",
							ast->node.count);
					break;
				case x86_64:
					snprintf(assembly, *assembly_size,
							"  add $%d, %%r11\n",
							ast->node.count);
					break;
			}
			break;
		case AST_PTR_DEC:
			assembly = realloc_new_assembly(assembly, assembly_size, ptr_dec);
			if(!assembly) return NULL;
			switch(arch){
				case aarch64:
					snprintf(assembly, *assembly_size,
							"  sub x4, x4, %d\n",
							ast->node.count);
					break;
				case x86_64:
					snprintf(assembly, *assembly_size,
							"  sub $%d, %%r11\n",
							ast->node.count);
					break;
			}
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
