//<parser.c>::

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <tokens.h>
#include <parser.h>

// lookahead is the compiler's eye which is traditionally defined here
int             lookahead;
extern char lexeme[];// stores the token string content

void
E(void)
{	// check prefixing signal
	int sigflag = 0;
	int otimesflg = 0;
	int oplusflg = 0;

	if (lookahead == '+' || lookahead == '-') {
		if (lookahead == '-') {
			sigflag = 1; // signal seen
		}
		match(lookahead);
	}

	_T:
	// T();
	_F:
	// F();
	switch (lookahead) {
		case ID:
			fprintf(output, " %s", lexeme);
			match(ID);
			break;
		case DEC:
			match(DEC);
			break;
		case OCT:
			match(OCT);
			break;
		case HEX:
			match(HEX);
			break;
		default:
			match('(');
			E();
			match(')');
	}

	if (otimesflg) {
		fprintf(output, " %c", otimesflg);
		otimesflg = 0;
	}

	if (lookahead == '*' || lookahead == '/') {
		otimesflg = lookahead;
		match(lookahead);
		goto _F;
	}

	// term ends
	if (sigflag) {
		fprintf(output, " neg");
		sigflag = 0; // turn off sigflag
	}

	if (oplusflg) {
		fprintf(output, " %c", oplusflg);
		oplusflg = 0;
	}

	if (lookahead == '+' || lookahead == '-') {
		oplusflg = lookahead;
		match(lookahead);
		goto _T;
	}

}

/*
//  T -> F Q
void T(void)
{
_F:
	// F();

	switch(lookahead) {
		case ID:// it's a var'
			match(ID); break;
		case DEC:
			match(DEC); break;
		case OCT:
			match(OCT); break;
		case HEX:
			match(HEX); break;
		default:
			match('('); E(); match(')');
	}

	// {otimes F}
	if (lookahead == '*' || lookahead == '/') {
		match(lookahead); 
		goto _F;
	}
}
*/

//  R -> ['+''-'] T R | <empty>
// void R(void)
// {
// 	if (lookahead == '+' || lookahead == '-')
// 	{
// 		match(lookahead); T(); R(); // undesired tail recursion
// 	}
// 	else { ; }
// }

//  Q -> ['*''/'] F Q | <empty>
// void Q(void)
// {
// 	if (lookahead == '*' || lookahead == '/') { match(lookahead); F(); Q(); }
// 	else { ; }
// }

/*
//  F -> ID | DEC | '(' E ')'
void F(void)
{
	switch(lookahead) {
		case ID:// it's a var'
			match(ID); break;
		case DEC:
			match(DEC); break;
		case OCT:
			match(OCT); break;
		case HEX:
			match(HEX); break;
		default:
			match('('); E(); match(')');
	}
}
*/

void match(int required)
{
	if (lookahead == required) {
		lookahead = gettoken(source);
	} else {
		if (isprint(lookahead)) {
			fprintf(stderr, "token mismatch: %c while expected %c\nexiting with err\n",
					lookahead, required);
		} else {
			if (lookahead == -1) {
				fprintf(stderr, "premature EOF seen at line %d\n", line);
			} else {
				if ( isprint(lookahead) ) {
					fprintf(stderr,
						"token mismatch: %c while expected %c at line %d\nexiting with err\n",
						lookahead, required, line);
				} else {
					fprintf(stderr,
							"token mismatch: %d while expected %d at line %d\nexiting with err\n",
							lookahead, required, line);
				}
			}
		}
		exit(-2);
	}
}
