/*
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2025 Vgwws
 *
 * This file is licensed under MIT. See LICENSE for details.
 */

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
		case aarch32:
			fprintf(context.output,
					".global _start\n"
					".section .bss\n"
					"arr: .skip 30000\n"
					".section .text\n"
					"_start:\n"
					"ldr r3, =arr\n"
					"mov r4, #0\n"
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
		case i386:
			fprintf(context.output,
					".global _start\n"
					".section .bss\n"
					"arr: .skip 30000\n"
					".section .text\n"
					"_start:\n"
					"movl $arr, %%eax\n"
					"xor %%ebx, %%ebx\n"
					);
			break;
		default:
			break;
	}
	for(unsigned int i = 0; i < ast->child_count; i++)
		generate_asm(ast->child_nodes[i], context);
	switch(context.arch){
		case aarch64:
			fprintf(context.output,
					"mov x8, 93\n"
					"mov x0, 0\n"
					"svc 0\n"
					);
			break;
		case aarch32:
			fprintf(context.output,
					"mov r7, #1\n"
					"mov r0, #0\n"
					"svc #0\n"
					);
			break;
		case x86_64:
			fprintf(context.output,
					"mov $60, %%rax\n"
					"mov $0, %%rdi\n"
					"syscall\n"
					);
			break;
		case i386:
			fprintf(context.output,
					"movl $1, %%eax\n"
					"xor %%ebx, %%ebx\n"
					"int $0x80\n"
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
		case aarch32:
			fprintf(context.output,
					"loop%d_start:\n"
					"ldrb r0, [r3, r4]\n"
					"cmp r0, #0\n"
					"beq loop%d_end\n",
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
		case i386:
			fprintf(context.output,
					"loop%d_start:\n"
					"movzbl (%%eax, %%ebx), %%ecx\n"
					"test %%ecx, %%ecx\n"
					"jz loop%d_end\n",
					depth, depth
					);
			break;
		default:
			break;
	}
	depth++;
	for(unsigned int i = 0; i < ast->child_count; i++)
		generate_asm(ast->child_nodes[i], context);
	switch(context.arch){
		case aarch64:
		case aarch32:
			fprintf(context.output,
					"b loop%d_start\n"
					"loop%d_end:\n",
					depth - 1, depth - 1
					);
			break;
		case x86_64:
		case i386:
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

void generate_mul(AST* ast, Context context){
	const char* move = ast->node.type == AST_MUL_INC ? "add" : "sub";
	switch(context.arch){
		case aarch64:
			fprintf(context.output,
					"mov w1, %d\n"
					"ldrb w0, [x3, x4]\n"
					"mul w0, w0, w1\n"
					"%s x5, x4, %d\n"
					"strb w0, [x3, x5]\n",
					ast->node.count, move, ast->num
					);
			break;
		case aarch32:
			fprintf(context.output,
					"mov r1, #%d\n"
					"ldrb r0, [r3, r4]\n"
					"mul r0, r0, r1\n"
					"%s r5, r4, #%d\n"
					"strb r0, [r3, r5]\n",
					ast->node.count, move, ast->num
					);
			break;
		case x86_64:
			fprintf(context.output,
					"mov %%r11, %%r12\n"
					"%s $%d, %%r12\n"
					"movb $%d, %%al\n"
					"movzxb (%%r10, %%r11), %%bl\n"
					"mulb %%bl\n"
					"movb %%al, (%%r10, %%r12)\n",
					move, ast->num, ast->node.count
					);
			break;
		case i386:
			fprintf(context.output,
					"mov %%ebx, %%ecx\n"
					"%s $%d, %%ecx\n"
					"movb $%d, %%al\n"
					"movzxb (%%eax, %%ebx), %%bl\n"
					"mulb %%bl\n"
					"movb %%al, (%%eax, %%ecx)\n",
					move, ast->num, ast->node.count
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
					"mov x8, 64\n"
					"mov x0, 1\n"
					"add x1, x3, x4\n"
					"mov x2, 1\n"
					"svc 0\n"
					);
			break;
		case aarch32:
			fprintf(context.output,
					"mov r7, #4\n"
					"mov r0, #1\n"
					"add r1, r3, r4\n"
					"mov r2, #1\n"
					"svc #0\n"
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
		case i386:
			fprintf(context.output,
					"pushl %%eax\n"
					"pushl %%ebx\n"
					"leal (%%eax, %%ebx), %%ecx\n"
					"movl $4, %%eax\n"
					"movl $1, %%ebx\n"
					"movl $1, %%edx\n"
					"int $0x80\n"
					"popl %%ebx\n"
					"popl %%eax\n"
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
					"mov x8, 63\n"
					"mov x0, 0\n"
					"add x1, x3, x4\n"
					"mov x2, 1\n"
					"svc 0\n"
					);
			break;
		case aarch32:
			fprintf(context.output,
					"mov r7, #3\n"
					"mov r0, #0\n"
					"add r1, r3, r4\n"
					"mov r2, #1\n"
					"svc #0\n"
					);
			break;
		case x86_64:
			fprintf(context.output,
					"push %%r11\n"
					"mov $0, %%rax\n"
					"mov $0, %%rdi\n"
					"lea arr(%%rip), %%rsi\n"
					"add %%r11, %%rsi\n"
					"mov $1, %%rdx\n"
					"syscall\n"
					"pop %%r11\n"
					);
			break;
		case i386:
			fprintf(context.output,
					"pushl %%eax\n"
					"pushl %%ebx\n"
					"leal (%%eax, %%ebx), %%ecx\n"
					"movl $3, %%eax\n"
					"movl $0, %%ebx\n"
					"movl $1, %%edx\n"
					"int $0x80\n"
					"popl %%ebx\n"
					"popl %%eax\n"
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
		case aarch32:
			fprintf(context.output,
					"ldrb r0, [r3, r4]\n"
					"add r0, r0, #%d\n"
					"strb r0, [r3, r4]\n",
					ast->node.count
					);
			break;
		case x86_64:
			fprintf(context.output,
					"addb $%d, (%%r10, %%r11)\n",
					ast->node.count
					);
			break;
		case i386:
			fprintf(context.output,
					"addb $%d, (%%eax, %%ebx)\n",
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
		case aarch32:
			fprintf(context.output,
					"ldrb r0, [r3, r4]\n"
					"sub r0, r0, #%d\n"
					"strb r0, [r3, r4]\n",
					ast->node.count
					);
			break;
		
		case x86_64:
			fprintf(context.output,
					"subb $%d, (%%r10, %%r11)\n",
					ast->node.count
					);
			break;
		case i386:
			fprintf(context.output,
					"subb $%d, (%%eax, %%ebx)\n",
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
		case aarch32:
			fprintf(context.output,
					"add r4, r4, #%d\n",
					ast->node.count
					);
			break;
		case x86_64:
			fprintf(context.output,
					"add $%d, %%r11\n",
					ast->node.count
					);
			break;
		case i386:
			fprintf(context.output,
					"add $%d, %%ebx\n",
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
		case aarch32:
			fprintf(context.output,
					"sub r4, r4, #%d\n",
					ast->node.count
					);
			break;
		case x86_64:
			fprintf(context.output,
					"sub $%d, %%r11\n",
					ast->node.count
					);
			break;
		case i386:
			fprintf(context.output,
					"sub $%d, %%ebx\n",
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
		case AST_MUL_INC:
		case AST_MUL_DEC:
			generate_mul(ast, context);
			break;
		default:
			break;
	}
}
	
