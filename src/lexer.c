/*
 * SPDX-License-Identifier: MIT
 * Copyright (C) 2025 Vgwws
 *
 * This file is licensed under MIT. See LICENSE for details.
 */

#include <stdio.h>
#include <stdlib.h>

#include "bfc_lexer.h"

Lexer* lexer_init(const char* source){
	Lexer* lexer = malloc(sizeof(Lexer));
	lexer->index = 0;
	lexer->current_char = source[0];
	lexer->token.count = 0;
	return lexer;
}

void lexer_advance(Lexer* lexer, const char* source){
	char ch = lexer->current_char;
	if(ch == '\0'){
		lexer->token.type = TOKEN_EOF;
		lexer->token.count = 1;
		return;
	}
	int net = 0;
	short is_ptr_op = 0;
	short is_val_op = 0;
	short is_output_input = 0;
	char f_op = 0;
	char s_op = 0;
	if(ch == '+' || ch == '-'){
		is_val_op = 1;
		f_op = '+';
		s_op = '-';
	}
	else if(ch == '>' || ch == '<'){
		is_ptr_op = 1;
		f_op = '>';
		s_op = '<';
	}
	else if(ch == '.' || ch == ','){
		is_output_input = 1;
		f_op = '.';
		s_op = ',';
	}
	while((lexer->current_char == f_op || lexer->current_char == s_op) &&
			(is_val_op || is_ptr_op || is_output_input)){
		if(lexer->current_char == '.' || lexer->current_char == ',')
			net++;
		if((lexer->current_char == '.' || lexer->current_char == ',') && source[lexer->index + 1] != '>'){
			lexer->current_char = source[++lexer->index];
			break;
		}
		if(lexer->current_char == '+' || lexer->current_char == '>')
			net++;
		else if(lexer->current_char == '-' || lexer->current_char == '<')
			net--;
		lexer->current_char = source[++lexer->index];
	}
	if(is_ptr_op || is_val_op || is_output_input){
		if(is_output_input){
			lexer->token.count = net;
			if(ch == '.')
				lexer->token.type = TOKEN_OUTPUT;
			else
				lexer->token.type = TOKEN_INPUT;
		}
		else if(net < 0){
			lexer->token.count = -net;
			if(is_ptr_op){
				lexer->token.type = TOKEN_PTR_DEC;
			}
			else{
				lexer->token.type = TOKEN_VAL_DEC;
			}
		}
		else if(net > 0){
			lexer->token.count = net;
			if(is_ptr_op){
				lexer->token.type = TOKEN_PTR_INC;
			}
			else{
				lexer->token.type = TOKEN_VAL_INC;
			}
		}
		if(!net)
			lexer_advance(lexer, source);
		return;
	}
	lexer->current_char = source[++lexer->index];
	switch(ch){
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
			lexer_advance(lexer, source);
			break;
	}
}
