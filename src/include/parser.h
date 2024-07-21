#ifndef PARSER_H
#define PARSER_H

#include "stack.h"
#include "types.h"
#include "lexer.h"
#include "bank.h"

typedef struct {
	char* name;
	u16 token_index;
} Label;

Label* init_label(char* name, u16 token_index);

typedef struct {
	u16 cur;
	Token** tokens;
	u16 token_size;
	Label** labels;
	u16 label_size;
	Stack* top;
	u16 stack_size;

	Bank* bank;
} Parser;

Parser* init_parser(Lexer* lexer);
void parse_tokens(Parser* parser);
void parse_token(Parser* parser);
bool expect_token(Parser* parser, int type);
Token* peek_token(Parser* parser);
Token* peek_next_token(Parser* parser);
Token* advance_token(Parser* parser);
bool is_at_end_token(Parser* parser);
void go_to_label(Parser* parser, char* label_name);
void skip_else(Parser* parser);

#endif