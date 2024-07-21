#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/lexer.h"
#include "include/parser.h"

void run(char* source);
void run_prompt();

char* read_file(const char* path)
{
	FILE* file = fopen(path, "r");
	if (file) {
		fseek(file, 0, SEEK_END);
		uint16_t len = ftell(file);
		char* content = calloc(len + 1, sizeof(char));
		fseek(file, 0, SEEK_SET);
		if (content) {
			fread(content, sizeof(char), len, file);
		}
		fclose(file);
		content[len] = '\0';
		return content;
	} else {
		printf("Error: no file found whose name is %s\n", path);
	}
	return (void*)0;
}

void run_prompt() {
	while (1) {
		printf(">> ");
		char line[100];
		if (!fgets(line, sizeof(line), stdin)) {
			printf("\n");
			break;
		}
		line[strlen(line) - 1] = '\0';
		run(line);
	}
}

void run(char* source)
{
	Lexer* lexer = init_lexer(source);
	collect_tokens(lexer);
	Parser* parser = init_parser(lexer);
	parse_tokens(parser);
	free(parser);
}

bool is_snof_file(const char* path)
{
	char ext[16];
	bool is_end = false;
	u8 len = 1;
	for (int i = 0; path[i] != '\0'; i++) {
		if (is_end) {
			ext[len - 1] = path[i];
			ext[len] = '\0';
			len++;
			if (len > 15)
				break;
			continue; 
		}
		if (path[i] == '.' && is_end == false) {
			is_end = true;
		}
	}
	return strcmp(ext, "snof") == 0;
}

int main(int argc, char** argv)
{
	if (argc > 1) {
		if (is_snof_file(argv[1])) {
			run(read_file(argv[1]));
		} else {
			printf("\"%s\" is not a snof script file\n", argv[1]);
			return 1;
		}
	} else {
		printf("Input file path\n");
		return 1;
	}
	return 0;
}