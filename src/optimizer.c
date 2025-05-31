/*
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2025 Vgwws
 *
 * This file is licensed under MIT. See LICENSE for details.
 */

#include <stdio.h>
#include <stdlib.h>

#include "bfc_optimizer.h"

short error_flag = 0;

void to_mul(AST** ast_ptr){
	AST* ast = *ast_ptr;
	if(ast->node.type != AST_LOOP)
		return;
	AST** ast_mul = ast->child_nodes;
	if(!ast_mul)
		return;
	if(ast->child_count != 4)
		return;
	if(ast_mul[0]->node.type != AST_VAL_DEC && ast_mul[0]->node.count > 1)
		return;
	if(ast_mul[1]->node.type != AST_PTR_INC && ast_mul[1]->node.type != AST_PTR_DEC)
		return;
	if(ast_mul[2]->node.type != AST_VAL_INC && ast_mul[2]->node.type != AST_VAL_DEC)
		return;
	if((ast_mul[1]->node.type == AST_PTR_INC && ast_mul[3]->node.type != AST_PTR_DEC) || 
			(ast_mul[1]->node.type == AST_PTR_DEC && ast_mul[3]->node.type != AST_PTR_INC))
		return;
	AST* new_ast = calloc(1, sizeof(AST));
	if(!new_ast){
		fprintf(stderr, "ERROR: Memory allocation for ast failed\n");
		error_flag = 1;
		return;
	}
	new_ast->node.type = ast_mul[1]->node.type == AST_PTR_INC ? AST_MUL_INC : AST_MUL_DEC;
	new_ast->node.count = ast_mul[2]->node.count;
	new_ast->num = ast_mul[1]->node.count;
	new_ast->child_nodes = NULL;
	new_ast->capacity = 0;
	new_ast->child_count = 0;
	clean_ast(ast);
	*ast_ptr = new_ast;
}

int optimize_ast(AST* ast){
	for(unsigned int i = 0; i < ast->child_count; i++){
		to_mul(&ast->child_nodes[i]);
		if(error_flag)
			return 1;
	}
	return 0;
}
