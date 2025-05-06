#ifndef BFC_PARSER_H
#define BFC_PARSER_H

#include "bfc_lexer.h"

typedef enum {
	AST_PROGRAM,
	AST_VAL_INC,
	AST_VAL_DEC,
	AST_PTR_INC,
	AST_PTR_DEC,
	AST_INPUT,
	AST_OUTPUT,
	AST_LOOP
} ASTNodeType;

typedef struct {
	ASTNodeType type;
	unsigned int count;
} ASTNode;

typedef struct AST {
	ASTNode node;
	struct AST** child_nodes;
	unsigned int child_count;
	unsigned int capacity;
} AST;

typedef struct {
	Token current_token;
	unsigned int index;
	short error_flag;
} Parser;

Parser* parser_init(const Token* tokens);

AST* parse_program(Parser* parser, const Token* tokens);

void clean_ast(AST* ast);

#endif
