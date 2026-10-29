#include <stdio.h>
#include <tokens.h>
#include <lexer.h>
#include <parser.h>
#include <main.h>

FILE *source, *output;

int main(int argc, char const *argv[])
{
	if (argc > 1) {
		source = fopen(argv[1], "r");
		if (source == NULL) {
			fprintf(stderr, "Erro ao abrir o arquivo de entrada\n");
			return 1;
		}
	} else {
		source = stdin;
	}
	output = stdout;

	lookahead = gettoken(source);
	program();

	return 0;
}
