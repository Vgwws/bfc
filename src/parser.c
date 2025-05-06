/* This source code is licensed under MIT License */

#include <stdio.h>
#include <stdlib.h>

#include "bfc_parser.h"
#include "bfc_lexer.h"

#define CHILDS_SIZE 10

/* Function prototype */

AST* parse_commands(Parser* parser, const Token* tokens);
AST* parse_command(Parser* parser, const Token* tokens);
AST* parse_loop(Parser* parser, const Token* tokens);

Parser* parser_init(const Token* tokens){
	Parser* parser = malloc(sizeof(Parser));
	parser->index = 0;
	parser->current_token = tokens[0];
	parser->error_flag = 0;
	return parser;
}

AST* parse_program(Parser* parser, const Token* tokens){
	AST* ast = parse_commands(parser, tokens);
	if(!ast) return NULL;
	if(parser->current_token.type == TOKEN_CLOSE_LOOP){
		fprintf(stderr, "ERROR: Unexpected ]\n");
		parser->error_flag = 1;
		clean_ast(ast);
		return NULL;
	}
	ast->node.type = AST_PROGRAM;
	return ast;
}

AST* parse_commands(Parser* parser, const Token* tokens){
	AST* ast = malloc(sizeof(AST));
	if(!ast){
		fprintf(stderr, "ERROR: Memory allocation for ast failed\n");
		parser->error_flag = 1;
		return NULL;
	}
	ast->child_nodes = malloc(CHILDS_SIZE * sizeof(AST*));
	if(!ast->child_nodes){
		fprintf(stderr, "ERROR: Memory allocation for ast failed\n");
		parser->error_flag = 1;
		free(ast);
		return NULL;
	}
	ast->capacity = CHILDS_SIZE;
	ast->child_count = 0;
	while(parser->current_token.type != TOKEN_EOF &&
			parser->current_token.type != TOKEN_CLOSE_LOOP){
		ast->child_count++;
		if(ast->child_count >= ast->capacity){
			ast->capacity += CHILDS_SIZE;
			AST** new_child_nodes = realloc(ast->child_nodes,
					ast->capacity * sizeof(AST*));
			if(!new_child_nodes){
				fprintf(stderr, "ERROR: Memory allocation for ast failed\n");
				parser->error_flag = 1;
				clean_ast(ast);
				return NULL;
			}
			ast->child_nodes = new_child_nodes;
		}
		ast->child_nodes[ast->child_count - 1] = parse_command(parser, tokens);
		if(parser->error_flag){
			clean_ast(ast);
			return NULL;
		}
	}
	return ast;
}

AST* parse_command(Parser* parser, const Token* tokens){
	if(parser->current_token.type == TOKEN_OPEN_LOOP){
		parser->current_token = tokens[++parser->index];
		AST* ast = parse_commands(parser, tokens);
		if(parser->current_token.type != TOKEN_CLOSE_LOOP){
			fprintf(stderr, "ERROR: Unclosed [\n");
			parser->error_flag = 1;
			return ast;
		}
		parser->current_token = tokens[++parser->index];
		if(!ast) return NULL;
		ast->node.type = AST_LOOP;
		return ast;
	}
	AST* ast = malloc(sizeof(AST));
	if(!ast){
		fprintf(stderr, "ERROR: Memory allocation for ast failed\n");
		parser->error_flag = 1;
		return NULL;
	}
	switch(parser->current_token.type){
		case TOKEN_VAL_INC:
			ast->node.type = AST_VAL_INC;
			break;
		case TOKEN_VAL_DEC:
			ast->node.type = AST_VAL_DEC;
			break;
		case TOKEN_PTR_INC:
			ast->node.type = AST_PTR_INC;
			break;
		case TOKEN_PTR_DEC:
			ast->node.type = AST_PTR_DEC;
			break;
		case TOKEN_INPUT:
			ast->node.type = AST_INPUT;
			break;
		case TOKEN_OUTPUT:
			ast->node.type = AST_OUTPUT;
			break;
		default:
			free(ast);
			return NULL;
	}
	ast->node.count = parser->current_token.count;
	parser->current_token = tokens[++parser->index];
	return ast;
}

void clean_ast(AST* ast){
	if(!ast) return;
	for(unsigned int i = 0; i < ast->child_count; i++){
		clean_ast(ast->child_nodes[i]);
	}
	if(ast->child_nodes) free(ast->child_nodes);
	free(ast);
}
