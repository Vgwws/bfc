#ifndef BFC_LEXER_H
#define BFC_LEXER_H

typedef enum {
	TOKEN_VAL_INC,
	TOKEN_VAL_DEC,
	TOKEN_PTR_INC,
	TOKEN_PTR_DEC,
	TOKEN_OPEN_LOOP,
	TOKEN_CLOSE_LOOP,
	TOKEN_INPUT,
	TOKEN_OUTPUT,
	TOKEN_EOF
} TokenType;

typedef struct {
	TokenType type;
	unsigned int count;
} Token;

typedef struct {
	Token token;
	char current_char;
	unsigned int index;
} Lexer;

Lexer* lexer_init(const char* source);

void lexer_advance(Lexer* lexer, const char* source);

#endif
