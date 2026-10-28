// Parser descendente recursivo da sintaxe MyPas.

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <tokens.h>
#include <parser.h>
#include <lexer.h>
#include <symtab.h>
#include <keywords.h>

int lookahead;
int has_error = 0;

static const char *token_name(int token)
{
	static char buf[16];

	if (token >= BEGIN && token <= END)
		return keywordlist[token - BEGIN];

	switch (token) {
		case ID:   return "ID";
		case UINT:  return "UINT";
		case FLT:  return "FLT";
		case ASGN: return "':='";
		case LEQ:  return "'<='";
		case NEQ:  return "'<>'";
		case GEQ:  return "'>='";
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
	if (lookahead >= BEGIN && lookahead <= END) {
		snprintf(buf, n, "%s", keywordlist[lookahead - BEGIN]);
		return;
	}
	if (lookahead == ASGN) {
		snprintf(buf, n, "':='");
		return;
	}
	if (lookahead == LEQ) {
		snprintf(buf, n, "'<='");
		return;
	}
	if (lookahead == NEQ) {
		snprintf(buf, n, "'<>'");
		return;
	}
	if (lookahead == GEQ) {
		snprintf(buf, n, "'>='");
		return;
	}
	if (lookahead == ID || lookahead == UINT || lookahead == FLT) {
		snprintf(buf, n, "'%s'", lexeme);
		return;
	}
	if (isprint(lookahead)) {
		snprintf(buf, n, "'%c'", lookahead);
		return;
	}
	snprintf(buf, n, "%s", token_name(lookahead));
}

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

void program(void) // checked
{
	match(PROGRAM);
	match(ID);
	match('(');
	idlist(); // ok
	match(')');
	match(';');
	block();
	match('.');
}

void idlist(void) // checked
{
	//for each string naming variable or function, it must be inserted into the symbol table avoiding duplicates
_IDHEAD:
	//call symtab_append(const char *varname) before match
	if (has_error)
		return;
	match(ID);
	if (lookahead == ',') {
		match(',');
		goto _IDHEAD;
	}
}

void block(void)
{
	if (has_error)
		return;

	if (lookahead == VAR) {
		match(VAR);
	_idlist_head:
		if (has_error)
			return;
		idlist();
		match(':');
		type();
		match(';');
		if (lookahead == ID) {
			goto _idlist_head;
		}
	}
	while (lookahead == PROCEDURE || lookahead == FUNCTION) {
		int funcflg = (lookahead == FUNCTION);
		match(lookahead);
		// append subprogram name to symbol table regarding the specificity of the subprogram or function
		match(ID);
		parmlist();
		if (funcflg) {
			match(':');
			type();
		}
		match(';');
		block();
		match(';');
		if (has_error)
			return;
	}
	match(BEGIN);
	stmtlist();
	match(END);
}

void parmlist(void)
{
	if (lookahead == '(') {
		match('(');
	_again:
		if (has_error)
			return;
		if (lookahead == VAR) {
			match(VAR);
		}
		idlist(); // os simbolos declarados nesta instancia devm ter offset diferente declarado em block()
		match(':');
		type();
		if (lookahead == ';') {
			match(';');
			goto _again;
		}
		match(')');
	}
}

void stmtlist(void)
{
_stmt_head:
	if (has_error)
		return;
	stmt();
	if (lookahead == ';') {
		match(';');
		goto _stmt_head;
	}
}

void arglist(void)
{
	//for each string naming variable or function, it must be inserted into the symbol table avoiding duplicates
	_EXPRHEAD:
	//call symtab_append(const char *varname) before match
	if (has_error)
		return;
	if (lookahead == '(') {
		match('(');
		expr();
		if (lookahead == ',') {
			match(',');
			goto _EXPRHEAD;
		}
		match(')');
	}
}

void stmt(void)
{
	if (has_error)
		return;

	switch (lookahead) {
		case ID:
			match(ID);
			if (lookahead == ASGN) {
				match(ASGN);
				expr();
			} else {
				arglist();
			}
			break;
		case BEGIN:
			match(BEGIN);
			stmtlist();
			match(END);
			break;
		case IF:
			match(IF);
			expr();
			match(THEN);
			stmt();
			if (lookahead == ELSE) {
				match(ELSE);
				stmt();
			}
			break;
		case REPEAT:
			match(REPEAT);
			stmtlist();
			match(UNTIL);
			expr();
			break;
		case WHILE:
			match(WHILE);
			expr();
			match(DO);
			stmt();
			break;
		default:
			; // empty statement
	}
}

void type(void)
{
	switch (lookahead) {
		case INTEGER:
			match(INTEGER);
			break;
		case REAL:
			match(REAL);
			break;
		case DOUBLE:
			match(DOUBLE);
			break;
		default:
			match(BOOLEAN);
	}
}

int isrelop(void)
{
	switch (lookahead) {
		case '<':
		case LEQ:
		case NEQ:
		case '>':
		case GEQ:
		case '=':
			return lookahead;
	}
	return 0;
}

void expr(void) // expr types must be numerical or boolean
{
	if (has_error)
		return;
	smpexpr();
	if (isrelop()) {
		match(lookahead);
		smpexpr();
	}
}

int isotimes(void)
{
	if (lookahead == '*' || lookahead == '/' || lookahead == DIV ||
	    lookahead == MOD || lookahead == AND)
		return lookahead;
	return 0;
}

void smpexpr(void)
{
	if (has_error)
		return;

	if (lookahead == '+' || lookahead == '-') {
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
			match(ID); // ID is associated to a R-value or L-value variable
			if (lookahead == ASGN) {
				// ID := E -> L-value
				match(ASGN);
				expr();
			} else {
				// R-value
			}
			break;
		case FLT:
			match(FLT);
			break;
		case UINT:
			match(UINT);
			break;
		default:
			match('(');
			expr();
			match(')');
	}

	if (isotimes()) {
		match(lookahead);
		goto _F;
	}

	// term ends here

	if (lookahead == '+' || lookahead == '-') {
		match(lookahead);
		goto _T;
	}
}
