#include <stdio.h>
#include <ctype.h>
#include <tokens.h>
#include <main.h>

FILE *source;

int main(void)
{
	// source = fopen(argv[1], "r");

	source = stdin;

	lookahead = gettoken(stdin);

	E();

	return 0;
}
