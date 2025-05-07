/* This source code is licensed under MIT License */

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bfc_lexer.h"
#include "bfc_parser.h"
#include "bfc_asm.h"

#define TOKENS_SIZE 100

static inline void clean_up(
		char* assembly, AST* ast, Parser* parser, Token* tokens, Lexer* lexer){
	free(assembly);
	clean_ast(ast);
	free(parser);
	free(tokens);
	free(lexer);
}

char* read_file(const char* filename){
	FILE* file = fopen(filename, "r");
	if(!file){
		fprintf(stderr, "ERROR: Can't open file\n");
		return NULL;
	}
	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	rewind(file);
	char* buffer = malloc(length + 1);
	if(!buffer){
		fprintf(stderr, "ERROR: Memory allocation for file buffer failed\n");
		fclose(file);
		return NULL;
	}
	size_t bytes = fread(buffer, 1, length, file);
	if((long)bytes != length){
		fprintf(stderr, "ERROR: Can't fully read files\n");
		fclose(file);
		free(buffer);
		return NULL;
	}
	buffer[length] = '\0';
	return buffer;
}

int main(int argc, char** argv){
	char* output = "a.out";
	char* assembly_output = "progTEMP.s";
	char* input = NULL;
	char* target_option = NULL;
	char* cc = "gcc";
#if defined(__aarch64__)
	Arch target = aarch64;
#elif defined(__x86_64__)
	Arch target = x86_64;
#else
	fprintf(stderr, "Unsupported Architecture\n");
	return 1;
#endif
	int opt;
	short output_assembly = 0;
	while((opt = getopt(argc, argv, "ho:S:t:")) != -1){
		switch(opt){
			case 'h':
				printf(
						"ebfc [OPTIONS] [FILENAME]\n"
						"OPTIONS:\n"
						"  -h : Print this Help\n"
						"  -o [FILENAME] : Specifying Output filename, if this option is omitted then default to 'a.out'\n"
						"  -S [FILENAME] : Specifying Output filename, Assembly Code\n"
						"  -t [ARCH] : Explicitly set assembly generation to use the specified CPU architecture, May need emulation\n"
						"  -c [PROGRAM] : Explicitly set assembler and linker program(example: GCC)\n");
				return 0;
			case 'o':
				output = optarg;
				break;
			case 'S':
				assembly_output = optarg;
				output_assembly = 1;
				break;
			case 't':
				target_option = optarg;
				if(strcmp(target_option, "aarch64") == 0){
					target = aarch64;
				}
				else if(strcmp(target_option, "x86_64") == 0){
					target = x86_64;
				}
				else{
					fprintf(stderr, "Unsupported Architecture\n");
					return 1;
				}
				break;
			case 'c':
				cc = optarg;
				break;
			case '?':
				fprintf(stderr, "Try ebfc -h for more information\n");
				return 1;
			default:
				abort();
		}
	}
	if(optind >= argc){
		fprintf(stderr, "ERROR: No input file\n");
		return 1;
	}
	if(optind + 1 < argc){
		fprintf(stderr, "ERROR: Can only specify one input file\n");
		return 1;
	}
	input = argv[optind];
	char* source = read_file(input);
	if(!source) return 1;
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
	if(parser->error_flag){
		clean_ast(ast);
		free(parser);
		free(tokens);
		free(lexer);
		return 1;
	}
	unsigned int assembly_size = ASM_SIZE;
	unsigned int index = 0;
	char* assembly = generate_asm(ast, &assembly_size, &index, target);
	FILE* file = fopen(assembly_output, "w");
	if(!file){
		fprintf(stderr, "ERROR: Can't open file\n");
		clean_up(assembly, ast, parser, tokens, lexer);
		return 1;
	}
	fwrite(assembly, 1, strlen(assembly), file);
	fclose(file);
	if(output_assembly){
		clean_up(assembly, ast, parser, tokens, lexer);
		return 0;
	}
	pid_t pid = fork();
	if(pid == -1){
		fprintf(stderr, "ERROR: Fork failed\n");
		clean_up(assembly, ast, parser, tokens, lexer);
		return 1;
	}
	else if(pid == 0){
		execlp(cc, cc, "-nostdlib", "-o", output, assembly_output, NULL);
		fprintf(stderr, "ERROR: Can't run subprocess\n");
		clean_up(assembly, ast, parser, tokens, lexer);
		return 1;
	}
	else{
		int status;
		waitpid(pid, &status, 0);
	}
	if(remove(assembly_output) != 0){
		fprintf(stderr, "ERROR: Can't remove temporary assembly files\n");
		clean_up(assembly, ast, parser, tokens, lexer);
		return 1;
	}
	clean_up(assembly, ast, parser, tokens, lexer);
	return 0;
}
