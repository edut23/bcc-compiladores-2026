#include <stdio.h>
#include <tokens.h>
#include <lexer.h>
#include <parser.h>
#include <main.h>

FILE *targetfile, *objfile;

int main(int argc, char const *argv[])
{
	const char *inputfname = NULL;
	const char *outputfname = "a.out";

	if (argc > 1) {
		inputfname = argv[1];
		targetfile = fopen(inputfname, "r");
		if (!targetfile) {
			fprintf(stderr, "file %s not found. Exiting with error status.\n",
				inputfname);
			return 1;
		}
	} else {
		targetfile = stdin;
	}

	if (argc > 2)
		outputfname = argv[2];

	objfile = fopen(outputfname, "w");
	if (!objfile) {
		fprintf(stderr, "cannot open output file %s. Exiting with error status.\n",
			outputfname);
		if (targetfile != stdin)
			fclose(targetfile);
		return 1;
	}

	lookahead = gettoken(targetfile);
	program();

	fclose(objfile);
	if (targetfile != stdin)
		fclose(targetfile);

	return 0;
}
