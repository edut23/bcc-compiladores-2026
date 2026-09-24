#include <stdio.h>
#include <ctype.h>
#include <tokens.h>
#include <main.h>

FILE *source, *output;

int main(void)
{
	// source = fopen(argv[1], "r");

	source = stdin;
	output = stdout;

	lookahead = gettoken(stdin);

	E();

	printf("\n");

	return 0;
}
