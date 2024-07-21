#ifndef LEXER_H
#define LEXER_H

#include "types.h"

typedef struct {
	enum {
		// FUNCTIONS
		PUSH, POP, ECHO, READ, EXIT, ADD, SUB, MUL, DIV, MOD, GOTO, SLEEP, SWAP,
		// BANK
		STORE, PULL, COPY, FREE, ADDR, BANK,
		// DATA TYPES
		EOF_TOKEN, STRING, LABEL, LABEL_CALL, INT, TOP, SECOND, SIZE, STACK,
		// BOOLEANS
		IFEMPTY, IFEQ, IFNE, IFGT, IFLT, ELSE,
	} type;
	char* value;
	u16 line;
} Token;

Token* init_token(int type, char* value, u16 line);
char* get_token_name(int type);

typedef struct {
	char* content;
	u32 cur;
	i32 line;
	u16 token_size;
	Token** tokens;
} Lexer;

Lexer* init_lexer(char* source);
void collect_tokens(Lexer* lexer);
void collect_token(Lexer* lexer);

void get_string(Lexer* lexer);
void get_id(Lexer* lexer);
void get_int(Lexer* lexer);
void skip_comment(Lexer* lexer);
void get_addr(Lexer* lexer);
char peek(Lexer* lexer);
char peek_next(Lexer* lexer);
char advance(Lexer* lexer);
char match(Lexer* lexer, char c);
bool is_at_end(Lexer* lexer);
void add_token(Lexer* lexer, Token* token);

#endif