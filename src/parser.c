#include "include/parser.h"
#include "include/lexer.h"
#include "include/bank.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// LABEL
Label* init_label(char* name, u16 token_index)
{
	Label* label = calloc(1, sizeof(Label));

	label->name = name;
	label->token_index = token_index;

	return label;
}

// PARSER
Parser* init_parser(Lexer* lexer)
{
	Parser* parser = calloc(1, sizeof(Parser));

	parser->cur = 0;
	parser->top = (void*)0;
	parser->stack_size = 0;
	parser->tokens = lexer->tokens;
	parser->token_size = lexer->token_size;

	u16 label_size = 0;
	parser->labels = (void*)0;

	parser->bank = init_bank();

	bool if_encountered = false;
	for (int i = 0; i < parser->token_size; i++) {
		bool is_newline = false;

		if (parser->tokens[i]->type == LABEL) {
			label_size++;
			parser->labels = realloc(parser->labels, label_size * sizeof(Label*));
			parser->labels[label_size - 1] = init_label(parser->tokens[i]->value, i);
		}

		// ADDR
		if (parser->tokens[i]->type == ADDR) {
			int val = atoi(parser->tokens[i]->value);
			if (val < 0 || val > 65535) {
				printf("Address interval : >= 0, < 65536; At line: %d\n", parser->tokens[i]->line);
				exit(1);
			}
		}

		if ((parser->tokens[i]->type == IFEQ || parser->tokens[i]->type == IFNE || parser->tokens[i]->type == IFGT ||
			parser->tokens[i]->type == IFLT || parser->tokens[i]->type == IFEMPTY) && if_encountered == false)
		{
			if_encountered = true;
		} else if ((parser->tokens[i]->type == IFEQ || parser->tokens[i]->type == IFNE || parser->tokens[i]->type == IFGT ||
			parser->tokens[i]->type == IFLT || parser->tokens[i]->type == IFEMPTY) && if_encountered == true && is_newline) {
			printf("Error: if after if at line: %d\n", parser->tokens[i]->line);
			exit(1);
		}
		if (parser->tokens[i]->type == ELSE && if_encountered == true) {
			if (parser->tokens[i + 1]->type == EOF_TOKEN || parser->tokens[i + 1]->line != parser->tokens[i]->line) {
				printf("Error: nothing after else at line: %d\n", parser->tokens[i]->line);
				exit(1);	
			}
			if_encountered = false;
		} else if (parser->tokens[i]->type == ELSE && if_encountered == false) {
			printf("Error: else without if at line: %d\n", parser->tokens[i]->line);
			exit(1);
		}


		if (parser->tokens[i]->type != EOF_TOKEN) {
			if (!is_newline && parser->tokens[i]->line != parser->tokens[i + 1]->line) {
				is_newline = true;
			} else {
				is_newline = false;
			}
		}
	}
	parser->label_size = label_size;

	for (int i = 0; i < parser->token_size; i++) {
		if (parser->tokens[i]->type != LABEL_CALL) continue;

			bool not_found = true;
			for (int j = 0; j < parser->label_size; j++) {
				if (strcmp(parser->tokens[i]->value, parser->labels[j]->name) == 0) {
					not_found = false;
					break;
				}
			}
			if (not_found) {
				printf("%s label is not defined\n", parser->tokens[i]->value);
				exit(1);
		}
	}

	free(lexer);
	return parser;
}

void parse_tokens(Parser* parser)
{
	while (parser->cur < parser->token_size) {
		parse_token(parser);
	}
}

void parse_token(Parser* parser)
{
	Token* token = advance_token(parser);

	switch (token->type) {
        case PUSH: {
	        if (peek_token(parser)->line == token->line) {
	        	Token* cur_token = peek_token(parser);
	        	switch (cur_token->type) {
	        		case INT:
	        			push(&parser->top, atoi(advance_token(parser)->value), &parser->stack_size);
	        			break;
	        		case TOP:
	        			push(&parser->top, peek_top(parser->top), &parser->stack_size);
	        			// SKIP TOP TOKEN AFTER PUSHING
	        			advance_token(parser);
	        			break;
	        		case SECOND:
	        			push(&parser->top, peek_second(parser->top), &parser->stack_size);
	        			// SKIP SECOND TOKEN AFTER PUSHING
	        			advance_token(parser);
	        			break;
	        		case SIZE:
	        			push(&parser->top, parser->stack_size, &parser->stack_size);
	        			// SKIP SIZE TOKEN AFTER PUSHING
	        			advance_token(parser);
	        			break;
	        		default:
	        			printf("Error: wrong type of token: %d after push at line: %d\n", cur_token->type, token->line);
						exit(1);
	        	}
	        }
			break;
		}
		case SLEEP: {
			if (peek_token(parser)->line == token->line) {
	        	Token* cur_token = peek_token(parser);
	        	switch (cur_token->type) {
	        		case INT:
	        			sleep_ms(atoi(advance_token(parser)->value));
	        			break;
	        		case TOP:
	        			sleep_ms(peek_top(parser->top));
	        			// SKIP TOP TOKEN
	        			advance_token(parser);
	        			break;
	        		case SECOND:
	        			sleep_ms(peek_second(parser->top));
	        			// SKIP SECOND TOKEN
	        			advance_token(parser);
	        			break;
	        		case SIZE:
	        			sleep_ms(parser->stack_size);
	        			// SKIP SIZE TOKEN
	        			advance_token(parser);
	        			break;
	        		default:
	        			printf("Error: wrong type of token: %d after sleep at line: %d\n", cur_token->type, token->line);
						exit(1);
	        	}
			} else {
				printf("Error: argument expected after sleep at line: %d\n", token->line);
				exit(1);
			}
			break;
		}
        case ECHO: {
	        if (peek_token(parser)->line == token->line) {
	        	Token* cur_token = peek_token(parser);
	        	switch (cur_token->type) {
	        		case STRING:
	        			printf("%s\n", advance_token(parser)->value);
	        			break;
	        		case TOP:
	        			printf("%d\n", peek_top(parser->top));
	        			// SKIP TOP TOKEN
	        			advance_token(parser);
	        			break;
	        		case SECOND:
	        			printf("%d\n", peek_second(parser->top));
	        			// SKIP SECOND TOKEN
	        			advance_token(parser);
	        			break;
	        		case SIZE:
	        			printf("%d\n", parser->stack_size);
	        			// SKIP SIZE TOKEN
	        			advance_token(parser);
	        			break;
	        		case STACK: {
	        			Stack* stack = parser->top;
	        			while (stack != (void*)0) {
	        				printf("%d\n", stack->data);
	        				stack = stack->prev;
	        			}
	        			// SKIP STACK TOKEN
	        			advance_token(parser);
	        			break;
					}
					case BANK:
						echo_bank(parser->bank);
						// SKIP BANK TOKEN
						advance_token(parser);
						break;
					case ADDR:
						printf("%d\n", (get_memory_data(parser->bank, atoi(advance_token(parser)->value))));
						break;
	        		default:
	        			printf("Error: wrong type of token: %d after echo at line: %d\n", cur_token->type, token->line);
						exit(1);
	        	}
			} else {
				printf("Error: argument expected after echo at line: %d\n", token->line);
				exit(1);
			}
			break;
		}
        case READ: {
			int in;
			scanf("%d", &in);
			push(&parser->top, in, &parser->stack_size);
			break;
        }
        case POP: {
			pop(&parser->top, &parser->stack_size);
			break;
		}
        case EXIT: {
			exit(0);
        	break;
        }
        case ADD: {
			push(&parser->top, pop(&parser->top, &parser->stack_size) + pop(&parser->top, &parser->stack_size), &parser->stack_size);
			break;
        }
		case SUB: {
			i32 first = pop(&parser->top, &parser->stack_size), second = pop(&parser->top, &parser->stack_size);
			push(&parser->top, second - first, &parser->stack_size);
			break;
		}
        case MUL: {
			push(&parser->top, pop(&parser->top, &parser->stack_size) * pop(&parser->top, &parser->stack_size), &parser->stack_size);
        	break;
        }
        case DIV: {
			i32 first = pop(&parser->top, &parser->stack_size), second = pop(&parser->top, &parser->stack_size);
			push(&parser->top, second / first, &parser->stack_size);
        	break;
        }
        case MOD: {
			i32 first = pop(&parser->top, &parser->stack_size), second = pop(&parser->top, &parser->stack_size);
			push(&parser->top, second % first, &parser->stack_size);
        	break;
        }
        case GOTO: {
        	if (peek_token(parser)->type == LABEL_CALL && peek_token(parser)->line == token->line) {
			go_to_label(parser, peek_token(parser)->value);
			} else {
				printf("Error: label expected after goto at line: %d\n", token->line);
				exit(1);
			}
			break;
		}
		case SWAP: {
			swap(&parser->top);
			break;
		}
		case IFEMPTY: {

			bool condition = false;
			
			if (peek_token(parser)->line == token->line) {
				Token* cur_token = peek_token(parser);
	        	
	        	switch (cur_token->type) {
	        		case TOP:
	        			if (is_empty(parser->top)) {
	        				condition = true;
	        			}
		        		advance_token(parser);
	        			break;
	        		case SECOND:
	        			if (is_empty(parser->top)) {
	        				condition = true;
	        			} else if (is_empty(parser->top->prev)) {
	        				condition = true;
	        			}
		        		advance_token(parser);
	        			break;
	        		case ADDR:
	        			if (is_memory_null(get_memory(parser->bank, atoi(advance_token(parser)->value)))) {
	        				condition = true;
	        			}
	        			break;
	        		default:
	        			printf("Error: wrong type of token: %s after %s at line: %d\n",
	        				   get_token_name(cur_token->type), get_token_name(token->type), token->line);
						exit(1);
	        	}
			} else {
				printf("Error: argument expected after boolean at line: %d\n", token->line);
				exit(1);
			}
			if (condition) {
				if (peek_token(parser)->line != token->line) {
					printf("Error: no command after %s at line: %d\n", get_token_name(token->type), token->line);
					exit(1);
				}
				// HANDLE ITSELF
				parse_token(parser);
				skip_else(parser);
			} else {
				while (peek_token(parser)->line == token->line) {
					advance_token(parser);
				}
				if (peek_token(parser)->type == ELSE) {
				// SKIP ELSE TOKEN
				advance_token(parser);
				parse_token(parser);
				}
			}
			break;
		}
		case IFEQ: {

			bool condition = false;
			
			if (peek_token(parser)->line == token->line) {
				Token* cur_token = peek_token(parser);
	        	
	        	switch (cur_token->type) {
	        		case INT:
	        			if (peek_top(parser->top) == atoi(advance_token(parser)->value)) {
	        				condition = true;
	        			}
	        			break;
	        		case SECOND:
	        			if (peek_top(parser->top) == peek_second(parser->top)) {
	        				condition = true;
	        			}
	        			// SKIP SECOND TOKEN
	        			advance_token(parser);
	        			break;
	        		case SIZE:
	        			if (peek_top(parser->top) == parser->stack_size) {
	        				condition = true;
	        			}
	        			// SKIP SIZE TOKEN
	        			advance_token(parser);
	        			break;
	        		case ADDR:
	        			if (peek_top(parser->top) == get_memory_data(parser->bank, atoi(advance_token(parser)->value))) {
	        				condition = true;
	        			}
	        			break;
	        		default:
	        			printf("Error: wrong type of token: %s after %s at line: %d\n",
	        				   get_token_name(cur_token->type), get_token_name(token->type), token->line);
	        			exit(1);
	        	}
			} else {
				printf("Error: argument expected after boolean at line: %d\n", token->line);
				exit(1);
			}
			if (condition) {
				if (peek_token(parser)->line != token->line) {
					printf("Error: no command after %s at line: %d\n", get_token_name(token->type), token->line);
					exit(1);
				}
				// HANDLE ITSELF
				parse_token(parser);
				skip_else(parser);
			} else {
				while (peek_token(parser)->line == token->line) {
					advance_token(parser);
				}
				if (peek_token(parser)->type == ELSE) {
				// SKIP ELSE TOKEN
				advance_token(parser);
				parse_token(parser);
				}
			}
			break;
		}
		case IFNE: {

			bool condition = false;
			
			if (peek_token(parser)->line == token->line) {
				Token* cur_token = peek_token(parser);
	        	
	        	switch (cur_token->type) {
	        		case INT:
	        			if (peek_top(parser->top) != atoi(advance_token(parser)->value)) {
	        				condition = true;
	        			}
	        			break;
	        		case SECOND:
	        			if (peek_top(parser->top) != peek_second(parser->top)) {
	        				condition = true;
	        			}
	        			// SKIP SECOND TOKEN
	        			advance_token(parser);
	        			break;
	        		case SIZE:
	        			if (peek_top(parser->top) != parser->stack_size) {
	        				condition = true;
	        			}
	        			// SKIP SIZE TOKEN
	        			advance_token(parser);
	        			break;
	        		case ADDR:
	        			if (peek_top(parser->top) != get_memory_data(parser->bank, atoi(advance_token(parser)->value))) {
	        				condition = true;
	        			}
	        			break;
	        		default:
	        			printf("Error: wrong type of token: %s after %s at line: %d\n",
	        				   get_token_name(cur_token->type), get_token_name(token->type), token->line);
	        			exit(1);
	        	}
			} else {
				printf("Error: argument expected after boolean at line: %d\n", token->line);
				exit(1);
			}
			if (condition) {
				if (peek_token(parser)->line != token->line) {
					printf("Error: no command after %s at line: %d\n", get_token_name(token->type), token->line);
					exit(1);
				}
				// HANDLE ITSELF
				parse_token(parser);
			} else {
				while (peek_token(parser)->line == token->line) {
					advance_token(parser);
				}
			}
			break;
		}
		case IFGT: {

			bool condition = false;
			
			if (peek_token(parser)->line == token->line) {
				Token* cur_token = peek_token(parser);
	        	
	        	switch (cur_token->type) {
	        		case INT:
	        			if (peek_top(parser->top) > atoi(advance_token(parser)->value)) {
	        				condition = true;
	        			}
	        			break;
	        		case SECOND:
	        			if (peek_top(parser->top) > peek_second(parser->top)) {
	        				condition = true;
	        			}
	        			// SKIP SECOND TOKEN
	        			advance_token(parser);
	        			break;
	        		case SIZE:
	        			if (peek_top(parser->top) > parser->stack_size) {
	        				condition = true;
	        			}
	        			// SKIP SIZE TOKEN
	        			advance_token(parser);
	        			break;
	        		case ADDR:
	        			if (peek_top(parser->top) > get_memory_data(parser->bank, atoi(advance_token(parser)->value))) {
	        				condition = true;
	        			}
	        			break;
	        		default:
	        			printf("Error: wrong type of token: %s after %s at line: %d\n",
	        				   get_token_name(cur_token->type), get_token_name(token->type), token->line);
	        			exit(1);
	        	}
			} else {
				printf("Error: argument expected after boolean at line: %d\n", token->line);
				exit(1);
			}
			if (condition) {
				if (peek_token(parser)->line != token->line) {
					printf("Error: no command after %s at line: %d\n", get_token_name(token->type), token->line);
					exit(1);
				}
				// HANDLE ITSELF
				parse_token(parser);
				skip_else(parser);
			} else {
				while (peek_token(parser)->line == token->line) {
					advance_token(parser);
				}
				if (peek_token(parser)->type == ELSE) {
				// SKIP ELSE TOKEN
				advance_token(parser);
				parse_token(parser);
				}
			}
			break;
		}
		case IFLT: {

			bool condition = false;
			
			if (peek_token(parser)->line == token->line) {
				Token* cur_token = peek_token(parser);
	        	
	        	switch (cur_token->type) {
	        		case INT:
	        			if (peek_top(parser->top) < atoi(advance_token(parser)->value)) {
	        				condition = true;
	        			}
	        			break;
	        		case SECOND:
	        			if (peek_top(parser->top) < peek_second(parser->top)) {
	        				condition = true;
	        			}
	        			// SKIP SECOND TOKEN
	        			advance_token(parser);
	        			break;
	        		case SIZE:
	        			if (peek_top(parser->top) < parser->stack_size) {
	        				condition = true;
	        			}
	        			// SKIP SIZE TOKEN
	        			advance_token(parser);
	        			break;
	        		case ADDR:
	        			if (peek_top(parser->top) < get_memory_data(parser->bank, atoi(advance_token(parser)->value))) {
	        				condition = true;
	        			}
	        			break;
	        		default:
	        			printf("Error: wrong type of token: %s after %s at line: %d\n",
	        				   get_token_name(cur_token->type), get_token_name(token->type), token->line);
	        			exit(1);
	        	}
			} else {
				printf("Error: argument expected after boolean at line: %d\n", token->line);
				exit(1);
			}
			if (condition) {
				if (peek_token(parser)->line != token->line) {
					printf("Error: no command after %s at line: %d\n", get_token_name(token->type), token->line);
					exit(1);
				}
				// HANDLE ITSELF
				parse_token(parser);
				skip_else(parser);
			} else {
				while (peek_token(parser)->line == token->line) {
					advance_token(parser);
				}
				if (peek_token(parser)->type == ELSE) {
				// SKIP ELSE TOKEN
				advance_token(parser);
				parse_token(parser);
				}
			}
			break;
		}
		case STORE: {
			if (peek_token(parser)->type == ADDR && token->line == peek_token(parser)->line) {
				store_memory(parser->bank, &parser->top, &parser->stack_size, atoi(advance_token(parser)->value));
			} else {
				printf("Error: address expected after store at line: %d\n", token->line);
				exit(1);
			}
			break;
		}
		case PULL: {
			if (peek_token(parser)->type == ADDR && token->line == peek_token(parser)->line) {
				pull_memory(parser->bank, &parser->top, &parser->stack_size, atoi(advance_token(parser)->value));
			} else {
				printf("Error: address expected after store at line: %d\n", token->line);
				exit(1);
			}
			break;
		}
		case COPY: {
			if (peek_token(parser)->type == ADDR && token->line == peek_token(parser)->line) {
				push(&parser->top, get_memory_data(parser->bank, atoi(advance_token(parser)->value)), &parser->stack_size);
			} else {
				printf("Error: address expected after store at line: %d\n", token->line);
				exit(1);
			}
			break;
		}
		case FREE: {
			if (peek_token(parser)->line == token->line) {
	        	Token* cur_token = peek_token(parser);
	        	switch (cur_token->type) {
	        		case ADDR:
	        			free_memory(parser->bank, atoi(advance_token(parser)->value));
	        			break;
	        		case STACK:
	        			clear(&parser->top, &parser->stack_size);
	        			// SKIP STACK TOKEN
	        			advance_token(parser);
	        			break;
	        		case BANK:
	        			free_bank(parser->bank);
	        			// SKIP TOP TOKEN
	        			advance_token(parser);
	        			break;
	        		default:
	        			printf("Error: wrong type of token: %s after %s at line: %d\n",
	        				   get_token_name(cur_token->type), get_token_name(token->type), token->line);
	        			exit(1);
	        	}
			} else {
				printf("Error: argument expected after free at line: %d\n", token->line);
				exit(1);
			}
			break;
		}
		case LABEL:
			break;
		case EOF_TOKEN:
			break;
		default:
			printf("Error: \"%s\" is not a command itself, at line: %d\n", get_token_name(token->type), token->line);
			exit(1);
			break;
    }
    if (parser->cur < parser->token_size) {
    	if (token->line == peek_token(parser)->line) {
			printf("Error: same line commands at line: %d\n", token->line);
	    	exit(1);
		}
    }
}

bool expect_token(Parser* parser, int type)
{
	if (peek_next_token(parser)->type == type) {
		advance_token(parser);
		return true;
	}
	return false;
}

Token* peek_token(Parser* parser)
{
	return parser->tokens[parser->cur];
}

Token* peek_next_token(Parser* parser)
{
	if (parser->cur + 1 >= parser->token_size)
		return (void*)0;
	return parser->tokens[parser->cur + 1];
}

Token* advance_token(Parser* parser)
{
	parser->cur++;
	return parser->tokens[parser->cur - 1];
}

void go_to_label(Parser* parser, char* label_name)
{
	for (int i = 0; i < parser->label_size; i++) {
		if (strcmp(parser->labels[i]->name, label_name) == 0) {
			parser->cur = parser->labels[i]->token_index;
			return;
		}
	}
	printf("Error: no such label named: \"%s\" at line: %d\n", label_name, parser->tokens[parser->cur]->line);
	exit(1);
}

void skip_else(Parser* parser)
{
	Token* start_token = peek_token(parser);
	if (start_token->type == ELSE) {
		while (peek_token(parser)->line == start_token->line) {
			advance_token(parser);
		}
	}
	if (peek_token(parser)->type == ELSE) {
		skip_else(parser);
	}
}
