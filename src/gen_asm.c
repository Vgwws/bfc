/* This source code is licensed under MIT License */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bfc_asm.h"

unsigned int depth = 0;

void generate_program(AST* ast, Context context){
	switch(context.arch){
		case aarch64:
			fprintf(context.output,
					".global _start\n"
					".section .bss\n"
					"arr: .skip 30000\n"
					".section .text\n"
					"_start:\n"
					"adr x3, arr\n"
					"mov x4, 0\n"
					);
			break;
		case x86_64:
			fprintf(context.output,
					".global _start\n"
					".section .bss\n"
					"arr: .skip 30000\n"
					".section .text\n"
					"_start:\n"
					"lea arr(%%rip), %%r10\n"
					"xor %%r11, %%r11\n"
					);
			break;
		default:
			break;
	}
	for(unsigned int i = 0; i < ast->child_count; i++){
		context.node_type = ast->child_nodes[i]->node.type;
		generate_asm(ast->child_nodes[i], context);
	}
	switch(context.arch){
		case aarch64:
			fprintf(context.output,
					"mov x0, 0\n"
					"mov x8, 93\n"
					"svc 0\n"
					);
			break;
		case x86_64:
			fprintf(context.output,
					"mov $60, %%rax\n"
				  "mov $0, %%rdi\n"
				  "syscall\n"
					);
			break;
		default:
			break;
	}				
}

void generate_loop(AST* ast, Context context){
	switch(context.arch){
		case aarch64:
			fprintf(context.output,
					"loop%d_start:\n"
					"ldrb w1, [x3, x4]\n"
					"cbz w1, loop%d_end\n",
					depth, depth
					);
			break;
		case x86_64:
			fprintf(context.output,
					"loop%d_start:\n"
					"movzbl (%%r10, %%r11), %%r9d\n"
					"test %%r9d, %%r9d\n"
					"jz loop%d_end\n",
					depth, depth
					);
			break;
		default:
			break;
	}
	depth++;
	for(unsigned int i = 0; i < ast->child_count; i++){
		context.node_type = ast->child_nodes[i]->node.type;
		generate_asm(ast->child_nodes[i], context);
	}
	switch(context.arch){
		case aarch64:
			fprintf(context.output,
					"b loop%d_start\n"
					"loop%d_end:\n",
					depth - 1, depth - 1
					);
			break;
		case x86_64:
			fprintf(context.output,
					"jmp loop%d_start\n"
					"loop%d_end:\n",
					depth - 1, depth - 1
					);
			break;
		default:
			break;
	}
}

void generate_output(Context context){
	switch(context.arch){
		case aarch64:
			fprintf(context.output,
					"mov x0, 1\n"
					"add x1, x3, x4\n"
					"mov x2, 1\n"
					"mov x8, 64\n"
					"svc 0\n"
					);
			break;
		case x86_64:
			fprintf(context.output,
					"push %%r11\n"
					"mov $1, %%rax\n"
					"mov $1, %%rdi\n"
					"lea arr(%%rip), %%rsi\n"
					"add %%r11, %%rsi\n"
					"mov $1, %%rdx\n"
					"syscall\n"
					"pop %%r11\n"
					);
			break;
		default:
			break;
	}
}

void generate_input(Context context){
	switch(context.arch){
		case aarch64:
			fprintf(context.output,
					"mov x0, 0\n"
					"add x1, x3, x4\n"
					"mov x2, 1\n"
					"mov x8, 63\n"
					"svc 0\n"
					);
			break;
		case x86_64:
			fprintf(context.output,
					"push %%r11\n"
					"push %%r10\n"
					"mov $0, %%rax\n"
					"mov $0, %%rdi\n"
					"lea arr(%%rip), %%rsi\n"
					"add %%r11, %%rsi\n"
					"mov $1, %%rdx\n"
					"syscall\n"
					"pop %%r11\n"
					);
			break;
		default:
			break;
	}
}

void generate_val_inc(AST* ast, Context context){
	switch(context.arch){
		case aarch64:
			fprintf(context.output,
					"ldrb w1, [x3, x4]\n"
					"add w1, w1, %d\n"
					"strb w1, [x3, x4]\n",
					ast->node.count
					);
			break;
		case x86_64:
			fprintf(context.output,
					"addb $%d, (%%r10, %%r11)\n",
					ast->node.count
					);
			break;
		default:
			break;
	}
}

void generate_val_dec(AST* ast, Context context){
	switch(context.arch){
		case aarch64:
			fprintf(context.output,
					"ldrb w1, [x3, x4]\n"
					"sub w1, w1, %d\n"
					"strb w1, [x3, x4]\n",
					ast->node.count
					);
			break;
		case x86_64:
			fprintf(context.output,
					"subb $%d, (%%r10, %%r11)\n",
					ast->node.count
					);
			break;
		default:
			break;
	}
}

void generate_ptr_inc(AST* ast, Context context){
	switch(context.arch){
		case aarch64:
			fprintf(context.output,
					"add x4, x4, %d\n",
					ast->node.count
					);
			break;
		case x86_64:
			fprintf(context.output,
					"add $%d, %%r11\n",
					ast->node.count
					);
			break;
		default:
			break;
	}
}

void generate_ptr_dec(AST* ast, Context context){
	switch(context.arch){
		case aarch64:
			fprintf(context.output,
					"sub x4, x4, %d\n",
					ast->node.count
					);
			break;
		case x86_64:
			fprintf(context.output,
					"sub $%d, %%r11\n",
					ast->node.count
					);
			break;
		default:
			break;
	}
}

void generate_asm(AST* ast, Context context){
	if(!ast) return;
	switch(ast->node.type){
		case AST_PROGRAM:
			generate_program(ast, context);
			break;
		case AST_LOOP:
			generate_loop(ast, context);
			break;
		case AST_PTR_INC:
			generate_ptr_inc(ast, context);
			break;
		case AST_PTR_DEC:
			generate_ptr_dec(ast, context);
			break;
		case AST_VAL_INC:
			generate_val_inc(ast, context);
			break;
		case AST_VAL_DEC:
			generate_val_dec(ast, context);
			break;
		case AST_INPUT:
			generate_input(context);
			break;
		case AST_OUTPUT:
			generate_output(context);
			break;
		default:
			break;
	}
}
	
