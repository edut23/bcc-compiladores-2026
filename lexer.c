// Reconhece identificadores, literais, :=, relacionais e ignora espaços/comentários.

#include <stdio.h>
#include <ctype.h>
#include <tokens.h>
#include <lexer.h>
#include <parser.h>
#include <keywords.h>

int line = 1;
char lexeme[MAXSTRLEN+1];

/* Espaços, inclusive quebra de linha (em Pascal newline não é token). */
void skipspaces(FILE *tape)
{
	int head;

	while (isspace(head = getc(tape))) {
		if (head == '\n')
			line++;
	}

	ungetc(head, tape);
}

/* Comentários Pascal `{ ... }`. `\{` e `\}` são literais. */
void skipcomments(FILE *tape)
{
	int head;
	int startline;

	while (1) {
		skipspaces(tape);
		if ((head = getc(tape)) != '{') {
			if (head != EOF)
				ungetc(head, tape);
			break;
		}

		startline = line;
		while ((head = getc(tape)) != '}') {
			if (head == '\\') {
				/* \{ e \} são literais; não fecham o comentário */
				head = getc(tape);
			}

			if (head == '\n')
				line++;

			if (head == EOF) { /* { sem } */
				fprintf(stderr, "comentário não fechado na linha %d\n", startline);
				has_error = 1;
				return;
			}
		}
	}
}

// ID = [A-Za-z][A-Za-z0-9]*
int isID(FILE *tape)
{
	if (isalpha((lexeme[0] = getc(tape)))) {
		int i = 1;
		while (isalnum(lexeme[i] = getc(tape))) {
			if (i < MAXSTRLEN)
				i++;
		}
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
		int token;
		if ((token = iskeyword(lexeme))) {
			return token;
		}
		return ID;
	}

	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}

// DEC = 0 | [1-9][0-9]*
int isDEC(FILE *tape)
{
	lexeme[0] = getc(tape);

	if (isdigit((unsigned char)lexeme[0])) {
		if (lexeme[0] == '0') {
			lexeme[1] = 0;
			return DEC;
		}
		int i = 1;
		while (isdigit((unsigned char)(lexeme[i] = getc(tape)))) i++;
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
		return DEC;
	}

	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}

// ASGN = ":="
int isASGN(FILE *tape)
{
	int head;

	if ((head = getc(tape)) == ':') {
		if ((head = getc(tape)) == '=') {
			lexeme[0] = ':';
			lexeme[1] = '=';
			lexeme[2] = 0;
			return ASGN;
		}
		ungetc(head, tape);
		head = ':';
	}
	ungetc(head, tape);
	return 0;
}

int isRELOP(FILE *tape)
{
	lexeme[0] = getc(tape);
	lexeme[1] = getc(tape);
	lexeme[2] = 0;

	switch (lexeme[0]) {
		case '<':
			switch (lexeme[1]) {
				case '=':
					return LEQ;
				case '>':
					return NEQ;
				default:
					ungetc(lexeme[1], tape);
					lexeme[1] = 0;
					return '<';
			}
		case '>':
			switch (lexeme[1]) {
				case '=':
					return GEQ;
				default:
					ungetc(lexeme[1], tape);
					lexeme[1] = 0;
					return '>';
			}
		case '=':
			ungetc(lexeme[1], tape);
			lexeme[1] = 0;
			return '=';
	}

	ungetc(lexeme[1], tape);
	ungetc(lexeme[0], tape);
	lexeme[1] = 0;
	return lexeme[0] = 0;
}

// NUM = [0-9]+ EE | [0-9]+\.[0-9]* | [0-9]*\.[0-9]+  ___ Transformar para isNum e nele verificar se é inteiro ou float chamando o isDec dentro dele, usar appendix
int isFLT(FILE *tape)
{

	if (isdigit(lexeme[0] = getc(tape))) {
		double aux;
		ungetc(lexeme[0], tape);
		fscanf(tape, "%lg", &aux);
		sprintf(lexeme, "%lg", aux);
		return FLT;
	}
	ungetc(lexeme[0], tape);
	lexeme[0] = 0;
	return 0;
}

int gettoken(FILE *src)
{
	int token;

	skipcomments(src); /* já ignora espaços e comentários em sequência */

	if ((token = isID(src))) return token;
	if ((token = isFLT(src))) return token;
	if ((token = isDEC(src))) return token;
	if ((token = isASGN(src))) return token;
	if ((token = isRELOP(src))) return token;

	token = getc(src);
	if (token == EOF) {
		lexeme[0] = 0;
	} else {
		lexeme[0] = token;
		lexeme[1] = 0;
	}
	return token;
}
