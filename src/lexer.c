#include <stdio.h>
#include <stdlib.h>

#include "ebfc_lexer.h"

Lexer* lexer_init(const char* source){
	Lexer* lexer = malloc(sizeof(Lexer));
	lexer->index = 0;
	lexer->current_char = source[0];
	return lexer;
}

void lexer_advance(Lexer* lexer, const char* source){
	char ch = lexer->current_char;
	if(ch == '\0'){
		lexer->token.type = TOKEN_EOF;
		lexer->token.count = 1;
		return;
	}
	lexer->token.count = 0;
	while(lexer->current_char == ch){
		lexer->current_char = source[++lexer->index];
		lexer->token.count++;
		if(ch == '[' || ch == ']' || ch == ',' || ch == '.'){
			break;
		}
	}
	int is_valid_token = 1;
	switch(ch){
		case '+':
			lexer->token.type = TOKEN_VAL_INC;
			break;
		case '-':
			lexer->token.type = TOKEN_VAL_DEC;
			break;
		case '>':
			lexer->token.type = TOKEN_PTR_INC;
			break;
		case '<':
			lexer->token.type = TOKEN_PTR_DEC;
			break;
		case ',':
			lexer->token.type = TOKEN_INPUT;
			break;
		case '.':
			lexer->token.type = TOKEN_OUTPUT;
			break;
		case '[':
			lexer->token.type = TOKEN_OPEN_LOOP;
			break;
		case ']':
			lexer->token.type = TOKEN_CLOSE_LOOP;
			break;
		default:
			is_valid_token = 0;
			break;
	}
	if(!is_valid_token){
		lexer_advance(lexer, source);
	}
}
