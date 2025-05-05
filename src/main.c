#include <stdio.h>
#include <stdlib.h>

#include "ebfc_lexer.h"
#include "ebfc_parser.h"

#define TOKENS_SIZE 100

int main(){
	const char* source = "++++>><<<[++mm---]";
	printf("%s\n", source);
	Lexer* lexer = lexer_init(source);
	Token* tokens = malloc(TOKENS_SIZE * sizeof(Token));
	int i = 0;
	do{
		lexer_advance(lexer, source);
		tokens[i].type = lexer->token.type;
		tokens[i].count = lexer->token.count;
		i++;
	}while(lexer->token.type != TOKEN_EOF);
	Parser* parser = parser_init(tokens);
	AST* ast = parse_program(parser, tokens);
	clean_ast(ast);
	free(tokens);
	free(parser);
	free(lexer);
	return 0;
}
