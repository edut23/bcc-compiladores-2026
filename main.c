#include <stdio.h>
#include <tokens.h>
#include <lexer.h>
#include <parser.h>
#include <main.h>

FILE *source, *output;

int main(void)
{
	source = stdin;
	output = stdout;

	lookahead = gettoken(source);
	mybc();

	return 0;
}
