#include "include/lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// TOKEN

Token* init_token(int type, char* value, u16 line)
{
	Token* token = calloc(1, sizeof(Token));
	token->line = line;
	token->value = value;
	token->type = type;

	return token;
}

char* get_token_name(int type)
{
	switch (type) {
//		PUSH, POP, ECHO, READ, EXIT, ADD, SUB, MUL, GOTO, SLEEP, SWAP
//		STRING, LABEL, LABEL_CALL, INT, TOP,
//		IFEMPTY, IFEMPTY1, IFEQ, IFGT, IFLT,		

		case PUSH: return "PUSH";
		case POP: return "POP";	
		case ECHO: return "ECHO";	
		case READ: return "READ";	
		case EXIT: return "EXIT";	
		case ADD: return "ADD";
		case SUB: return "SUB";
		case MUL: return "MUL";
		case GOTO: return "GOTO";
		case SLEEP: return "SLEEP";
		case SWAP: return "SWAP";
		case STRING: return "STRING";	
		case LABEL: return "LABEL";
		case LABEL_CALL: return "LABEL_CALL";
		case INT: return "INT";
		case TOP: return "TOP";
		case IFEMPTY: return "IFEMPTY";
		case IFEQ: return "IFEQ";
		case IFGT: return "IFGT";
		case IFLT: return "IFLT";
		case DIV: return "DIV";
		case MOD: return "MOD";
		case SECOND: return "SECOND";
		case SIZE: return "SIZE";
		case IFNE: return "IFNE";
		case STACK: return "STACK";
		case EOF_TOKEN: return "EOF";
		case STORE: return "STORE";
		case PULL: return "PULL";
		case FREE: return "FREE";
		case ADDR: return "ADDR";
		case BANK: return "BANK";
		case COPY: return "COPY";
		case ELSE: return "ELSE";

	}
	return (void*)0;
}

// LEXER

Lexer* init_lexer(char* source)
{
	Lexer* lexer = calloc(1, sizeof(Lexer));

	lexer->content = source;
	lexer->cur = 0;
	lexer->line = 1;
	lexer->token_size = 0;
	lexer->tokens = (void*)0;

	return lexer;
}

void collect_tokens(Lexer* lexer)
{
	while (!is_at_end(lexer)) {
		collect_token(lexer);
	}
	lexer->line++;
	add_token(lexer, init_token(EOF_TOKEN, (void*)0, lexer->line));
}

void collect_token(Lexer* lexer)
{
	char c = advance(lexer);

	if (c == ' ') {
		return;
	} else if (c == '\n') {
		lexer->line++;
	} else if (c == '$') {
		get_addr(lexer);
	} else if (isalpha(c)) {
		get_id(lexer);
	} else if (c == '"') {
		get_string(lexer);
	} else if (isdigit(c)) {
		get_int(lexer);
	} else if (c == '-') {
		if (isdigit(peek(lexer))) {
			get_int(lexer);
		}
	} else if (c == '@') {
		skip_comment(lexer);
	} else {
		printf("Unexpected character: %c at line: %d\n", c, lexer->line);
		exit(1);
	}
}

void get_string(Lexer* lexer)
{
	int len = 0;
	char* s = (void*)0;
	while (!is_at_end(lexer) && peek(lexer) != '"') {
		len++;
		s = realloc(s, sizeof(char) * (len + 1));
		s[len - 1] = advance(lexer);
		s[len] = '\0';
	}
	if (is_at_end(lexer)) {
		printf("Unterminated string at line: %d\n", lexer->line);
		return;
	}
	// for "
	advance(lexer);
	add_token(lexer, init_token(STRING, s, lexer->line));
}

void get_id(Lexer* lexer)
{
	int len = 1;
	char* s = calloc(len + 1, sizeof(char));
	s[0] = lexer->content[lexer->cur - 1];
	s[1] = '\0';

	while(!is_at_end(lexer) && peek(lexer) != ' ' && peek(lexer) != '\n' && peek(lexer) != ':') {
		len++;
		s = realloc(s, (len + 1) * sizeof(char));
		s[len - 1] = advance(lexer);
		s[len] = '\0';
	}

	int type = -1;
	if (peek(lexer) == ':') {
		advance(lexer);
		type = LABEL;
		add_token(lexer, init_token(type, s, lexer->line));
		return;
	}
	else if (strcmp(s, "PUSH") == 0) type = PUSH;
	else if (strcmp(s, "POP") == 0) type = POP;
	else if (strcmp(s, "ECHO") == 0) type = ECHO;
	else if (strcmp(s, "READ") == 0) type = READ;
	else if (strcmp(s, "EXIT") == 0) type = EXIT;
	else if (strcmp(s, "ADD") == 0) type = ADD;
	else if (strcmp(s, "SUB") == 0) type = SUB;
	else if (strcmp(s, "MUL") == 0) type = MUL;
	else if (strcmp(s, "GOTO") == 0) type = GOTO;
	else if (strcmp(s, "TOP") == 0) type = TOP;
	else if (strcmp(s, "SLEEP") == 0) type = SLEEP;
	else if (strcmp(s, "SWAP") == 0) type = SWAP;
	else if (strcmp(s, "IFEMPTY") == 0) type = IFEMPTY;
	else if (strcmp(s, "IFEQ") == 0) type = IFEQ;
	else if (strcmp(s, "IFGT") == 0) type = IFGT;
	else if (strcmp(s, "IFLT") == 0) type = IFLT;
	else if (strcmp(s, "DIV") == 0) type = DIV;
	else if (strcmp(s, "MOD") == 0) type = MOD;
	else if (strcmp(s, "SECOND") == 0) type = SECOND;
	else if (strcmp(s, "SIZE") == 0) type = SIZE;
	else if (strcmp(s, "IFNE") == 0) type = IFNE;
	else if (strcmp(s, "STACK") == 0) type = STACK;
	else if (strcmp(s, "STORE") == 0) type = STORE;
	else if (strcmp(s, "PULL") == 0) type = PULL;
	else if (strcmp(s, "FREE") == 0) type = FREE;
	else if (strcmp(s, "BANK") == 0) type = BANK;
	else if (strcmp(s, "COPY") == 0) type = COPY;
	else if (strcmp(s, "ELSE") == 0) type = ELSE;

	if (type > -1) {
		add_token(lexer, init_token(type, (void*)0, lexer->line));
	} else {
		add_token(lexer, init_token(LABEL_CALL, s, lexer->line));
		return;
	}
	free(s);
}

void get_int(Lexer* lexer)
{
	int len = 1;
	char* s = calloc(len + 1, sizeof(char));
	s[0] = lexer->content[lexer->cur - 1];
	s[1] = '\0';
	while (isdigit(peek(lexer))) {
		len++;
		s = realloc(s, (len + 1) * sizeof(char));
		s[len - 1] = advance(lexer);
		s[len] = '\0';
	}
	add_token(lexer, init_token(INT, s, lexer->line));
}

void skip_comment(Lexer* lexer)
{
	while (peek(lexer) != '\n' && !is_at_end(lexer)) {
		advance(lexer);
	}
}

void get_addr(Lexer* lexer)
{
	int len = 0;
	char* s = (void*)0;
	while (isdigit(peek(lexer))) {
		len++;
		s = realloc(s, (len + 1) * sizeof(char));
		s[len - 1] = advance(lexer);
		s[len] = '\0';
	}
	add_token(lexer, init_token(ADDR, s, lexer->line));
}

char peek(Lexer* lexer)
{
	if (is_at_end(lexer))
		return '\0';
	return lexer->content[lexer->cur];
}

char peek_next(Lexer* lexer)
{
	if (lexer->cur + 1 >= strlen(lexer->content))
		return '\0';
	return lexer->content[lexer->cur + 1];
}

char advance(Lexer* lexer)
{
	lexer->cur++;
	return lexer->content[lexer->cur - 1];
}

char match(Lexer* lexer, char c)
{
	return '\0';
}

bool is_at_end(Lexer* lexer)
{
	return lexer->cur >= strlen(lexer->content);
}

void add_token(Lexer* lexer, Token* token)
{
	lexer->token_size++;
	lexer->tokens = realloc(lexer->tokens, lexer->token_size * sizeof(Token*));
	lexer->tokens[lexer->token_size - 1] = token;
}
