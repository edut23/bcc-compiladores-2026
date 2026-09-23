#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <tokens.h>
#include <lexer.h>

int line = 1;

/*
	2026-09-09: Implementem um varredor de comentários Pascal [void skipcomments(FILE *)], onde
	um comentário começa com '{' e finaliza com '}', a menos que se utilize
	"\{" e "\}", para ser interpretado como elemento do texto.
	Exemplo:
	begin {
			isto é um comentário. Aqui, eu devo utilizar \{ e \} para constar no interior
		    do comentário
		  }     { outro comentário depois do comentário prévio }
	  blah blah blah
	end;
 */
/*
void skipcomments(FILE *tape)
{
	int head;
	while(1) {
		skipspaces(tape);
		if ( (head = getc(tape)) == '{') {
			while( (head = getc(tape)) != '}') {
				if (head == '\\') {
					if ( (head = getc(tape)) == '{' || head == '}')
						head = getc(tape);
				}
				if (head == '\n') {
					line++;
				}
				if ( head == EOF ) {
					fprintf(stderr, "unclosed comment at line %d\n", line);
					exit(-4);
				}
			}
		} else {
			ungetc(head, tape);
			break;
		}
	}
}
*/

void skipcomments(FILE *tape)
{
	int head;
	_skpspc:
	skipspaces(tape);
	if ( (head = getc(tape)) == '{') {
		while( (head = getc(tape)) != '}') {

			if (head == '\\') {
				if ( (head = getc(tape)) == '{' || head == '}')
					head = getc(tape);
			}

			if (head == '\n') {
				line++;
			}

			if ( head == EOF ) {
				fprintf(stderr, "unclosed comment at line %d\n", line);
				exit(-4);
			}
		}
		goto _skpspc;
	}
	ungetc(head, tape);
}

void skipspaces(FILE *tape)
{
	int head;

	while ( isspace(head = getc(tape)) ) {
		if (head == '\n') line++;
	}
	
	ungetc(head, tape);
}

// ID = [A-Za-z][A-Za-z0-9]*

int isID(FILE *tape)
{
	int head = getc(tape);

	if (isalpha(head)) {
		while(isalnum(head = getc(tape)));
		ungetc(head, tape);
		return ID;
	}
	ungetc(head, tape);

	return 0;
}

// DEC = 0 | [1-9][0-9]*

int isDEC(FILE *tape)
{
	int head = getc(tape);

	if (isdigit(head)) {
		if (head == '0') {
			return DEC;
		}
		while( isdigit(head = getc(tape)) );
		ungetc(head, tape);
		return DEC;
	}

	ungetc(head, tape);
	return 0;
}

// OCT = 0[0-7]+
int isOCT(FILE *tape)
{
	int head, prfx;

	if( (prfx = getc(tape)) == '0') {

		if ( '0' <= (head = getc(tape)) && head <= '7' ) {

			while ( '0' <= (head = getc(tape)) && head <= '7' );
			ungetc(head, tape);
			return OCT;

		}

		ungetc(head, tape);
	}

	ungetc(prfx, tape);
	return 0;
}

// HEX = 0[xX][0-9A-Fa-f]+
int isHEX(FILE *tape)
{
	char hexpref[2];
	if ( (hexpref[0] = getc(tape)) != '0' ) {
		ungetc(hexpref[0], tape);
		return 0;
	}
	// by now we got '0'
	if ( (toupper(hexpref[1]) != 'X') ) {
		ungetc(hexpref[1], tape);
		ungetc(hexpref[0], tape);
		return 0;
	}

	// at this point we got prefix "0[xX]"
	int head = getc(tape);

	// check wether head is in [0-9A-Fa-f]
	if (isxdigit(head)) {
		while ( isxdigit(head = getc(tape)) );
		ungetc(head, tape);
		return HEX;
	}

	// give back what is not a hex-digit
	ungetc(head, tape);

	// give back the hex prefix
	ungetc(hexpref[1], tape);
	ungetc(hexpref[0], tape);
	return 0;
}

// Main module (not main function) to call everything above
// int gettoken(FILE *); // must be visible to the parser
            
int gettoken(FILE *src)
{
	int token;
	
	// find the next non white character
	skipcomments(src);

	// variable name
	if ( (token = isID(src)) ) return token;
	// integer numbers
	if ( (token = isOCT(src)) ) return token;
	if ( (token = isHEX(src)) ) return token;
	if ( (token = isDEC(src)) ) return token;// is DEC must be the last on the possibility
											 // of zero ('0') be found
	// must be an ASCII character
	token = getc(src);

	return token;
}