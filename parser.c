// Parser descendente recursivo e interpretação das expressões.

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <tokens.h>
#include <parser.h>
#include <lexer.h>
#include <symtab.h>
#include <vmachine.h>

int lookahead;
int has_error = 0;

static const char *token_name(int token)
{
	static char buf[8];

	switch (token) {
		case ID:   return "ID";
		case DEC:  return "DEC";
		case OCT:  return "OCT";
		case HEX:  return "HEX";
		case ASGN: return "':='";
		case EOF:  return "EOF";
		default:
			if (isprint(token)) {
				buf[0] = '\'';
				buf[1] = (char)token;
				buf[2] = '\'';
				buf[3] = 0;
				return buf;
			}
			snprintf(buf, sizeof buf, "%d", token);
			return buf;
	}
}

static void describe_seen(char *buf, size_t n)
{
	if (lookahead == EOF) {
		snprintf(buf, n, "EOF");
		return;
	}
	if (lookahead == ASGN) {
		snprintf(buf, n, "':='");
		return;
	}
	if (lookahead == ID || lookahead == DEC || lookahead == OCT || lookahead == HEX) {
		snprintf(buf, n, "'%s'", lexeme);
		return;
	}
	if (isprint(lookahead)) {
		snprintf(buf, n, "'%c'", lookahead);
		return;
	}
	snprintf(buf, n, "%s", token_name(lookahead));
}

/* Descarta o resto da sentença até o delimitador. */
static void skip_to_stmt_end(void)
{
	while (lookahead != '\n' && lookahead != ';' && lookahead != EOF)
		lookahead = gettoken(source);
}

/* Consome o token esperado ou registra o erro, sem encerrar o processo. */
void match(int required)
{
	if (has_error)
		return;

	if (lookahead == required) {
		lookahead = gettoken(source);
		return;
	}

	has_error = 1;
	if (lookahead == EOF) {
		fprintf(stderr, "EOF prematuro na linha %d (esperado %s)\n",
			line, token_name(required));
	} else {
		char seen[MAXSTRLEN+4];
		describe_seen(seen, sizeof seen);
		fprintf(stderr, "token inesperado %s na linha %d (esperado %s)\n",
			seen, line, token_name(required));
	}
}

/* E → ['+'|'-'] T { ('+'|'-') T }  com T e F inlined. */
void expr(void)
{
	char name[MAXSTRLEN+1];
	int sigflag = 0;
	int otimesflg = 0;
	int oplusflg = 0;

	if (has_error)
		return;

	if (lookahead == '+' || lookahead == '-') {
		if (lookahead == '-')
			sigflag = 1;
		match(lookahead);
	}

_T:
	if (has_error)
		return;
_F:
	if (has_error)
		return;

	switch (lookahead) {
		case ID:
			strcpy(name, lexeme);
			match(ID);
			if (has_error)
				return;
			if (lookahead == ASGN) {
				match(ASGN);
				expr();
				if (!has_error)
					/*9*/ store(name, acc); /*9*/
			} else {
				/*8*/ acc = recall(name); /*8*/
			}
			break;
		case DEC:
			/*7*/ acc = atoi(lexeme); /*7*/
			match(DEC);
			break;
		case OCT:
			/*7*/ acc = strtol(lexeme, NULL, 8); /*7*/
			match(OCT);
			break;
		case HEX:
			/*7*/ acc = strtol(lexeme, NULL, 16); /*7*/
			match(HEX);
			break;
		default:
			match('(');
			expr();
			match(')');
	}

	if (has_error)
		return;

	if (otimesflg) {
		if (otimesflg == '*') {
			/*5*/ acc = pop() * acc; /*5*/
		} else if (otimesflg == '/') {
			if (acc == 0) {
				fprintf(stderr, "divisão por zero na linha %d\n", line);
				has_error = 1;
				return;
			}
			/*6*/ acc = pop() / acc; /*6*/
		}
		otimesflg = 0;
	}

	if (lookahead == '*' || lookahead == '/') {
		/*2*/ push(); /*2*/
		otimesflg = lookahead;
		match(lookahead);
		goto _F;
	}

	if (sigflag) {
		/*1*/ acc = -acc; /*1*/
		sigflag = 0;
	}

	if (oplusflg) {
		if (oplusflg == '+') {
			/*3*/ acc = pop() + acc; /*3*/
		} else if (oplusflg == '-') {
			/*4*/ acc = pop() - acc; /*4*/
		}
		oplusflg = 0;
	}

	if (lookahead == '+' || lookahead == '-') {
		/*2*/ push(); /*2*/
		oplusflg = lookahead;
		match(lookahead);
		goto _T;
	}
}

static int isFirst(void) /* token que pode iniciar uma expressão */
{
	switch (lookahead) {
		case '+':
		case '-':
		case ID:
		case DEC:
		case OCT:
		case HEX:
		case '(':
			return lookahead;
	}
	return 0;
}

/* Laço do interpretador: uma expressão por linha ou ponto-e-vírgula. */
void mybc(void)
{
	while (lookahead != EOF) {
		has_error = 0;
		sp = -1;

		if (isFirst()) {
			expr();
			if (!has_error && lookahead != '\n' && lookahead != ';' && lookahead != EOF) {
				char seen[MAXSTRLEN+4];
				describe_seen(seen, sizeof seen);
				fprintf(stderr, "token inesperado %s na linha %d\n", seen, line);
				has_error = 1;
			}
			if (!has_error)
				fprintf(output, "%lg\n", acc);
		} else if (lookahead != '\n' && lookahead != ';') {
			char seen[MAXSTRLEN+4];
			describe_seen(seen, sizeof seen);
			fprintf(stderr, "caractere inesperado %s na linha %d\n", seen, line);
			has_error = 1;
		}

		if (has_error)
			skip_to_stmt_end();

		has_error = 0;
		if (lookahead == '\n' || lookahead == ';')
			match(lookahead);
	}
}
