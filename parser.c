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
		lookahead = gettoken(targetfile);
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

int loopcount = 1;

void stmt(void)
{
	if (has_error)
		return;

	int lbl1, lbl2;
	char name[MAXSTRLEN+1];

	switch (lookahead) {
		case ID:
			strncpy(name, lexeme, MAXSTRLEN);
			name[MAXSTRLEN] = 0;
			match(ID);
			if (lookahead == ASGN) {
				match(ASGN);
				expr();
				fprintf(objfile, "\tmovl %%eax, %s\n", name);
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
			match(IF); // if head
			expr(); // <expr.as>
			/*1*/fprintf(objfile, "\tjz .L%d\n", lbl1 = lbl2 = loopcount++);/*1*/
			match(THEN);
			stmt(); // <stmt.as>
			if (lookahead == ELSE) {
				/*2*/
				// jmp .endiflabel
				fprintf(objfile, "\tjmp .L%d\n", lbl2 = loopcount++);
				// .elselabel
				fprintf(objfile, ".L%d:\n", lbl1);
				/*2*/
				match(ELSE);
				stmt(); // <stmt.as>
			}
			// .endiflabel
			/*3*/fprintf(objfile, ".L%d:\n", lbl2);/*3*/
			break;
		case REPEAT:
			// .repeatlabel
			match(REPEAT);
			/*1*/fprintf(objfile, ".L%d:\n", lbl1 = loopcount++);/*1*/
			stmtlist(); // <stmt.as>
			match(UNTIL);
			expr(); // <expr.as>
			/*2*/fprintf(objfile, "\tjz .L%d\n", lbl1);/*2*/
			// jz .repeatlabel
			break;
		case WHILE:
			// .while
			match(WHILE);
			/*1*/fprintf(objfile, ".L%d:\n", lbl1 = loopcount++);/*1*/
			expr(); // <expr.as>
			// jz .endwhile
			/*2*/fprintf(objfile, "\tjz .L%d\n", lbl2 = loopcount++);/*2*/
			match(DO);
			stmt(); // <stmt.as>
			// jmp .while
			/*3*/fprintf(objfile, "\tjmp .L%d\n", lbl1);/*3*/
			// .endwhile
			/*4*/fprintf(objfile, ".L%d:\n", lbl2);/*4*/
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
	int rel;

	if (has_error)
		return;
	smpexpr();
	if ((rel = isrelop())) {
		fprintf(objfile, "\tpushl %%eax\n");
		match(lookahead);
		smpexpr();
		/* left in (%esp) / after popl %ebx; right in %eax → 0/1 em %eax p/ jz */
		fprintf(objfile, "\tpopl %%ebx\n");
		fprintf(objfile, "\tcmpl %%eax, %%ebx\n");
		fprintf(objfile, "\tmovl $0, %%eax\n");
		switch (rel) {
			case '=':  fprintf(objfile, "\tsete %%al\n"); break;
			case NEQ:  fprintf(objfile, "\tsetne %%al\n"); break;
			case '<':  fprintf(objfile, "\tsetl %%al\n"); break;
			case LEQ:  fprintf(objfile, "\tsetle %%al\n"); break;
			case '>':  fprintf(objfile, "\tsetg %%al\n"); break;
			case GEQ:  fprintf(objfile, "\tsetge %%al\n"); break;
		}
	}
}

int isotimes(void)
{
	if (lookahead == '*' || lookahead == '/' || lookahead == DIV ||
	    lookahead == MOD || lookahead == AND)
		return lookahead;
	return 0;
}

static void apply_otimes(int op)
{
	switch (op) {
		case '*':
			fprintf(objfile, "\timull (%%esp), %%eax\n");
			fprintf(objfile, "\taddl $4, %%esp\n");
			break;
		case '/':
		case DIV:
			fprintf(objfile, "\tmovl %%eax, %%ebx\n");
			fprintf(objfile, "\tpopl %%eax\n");
			fprintf(objfile, "\tcltd\n");
			fprintf(objfile, "\tidivl %%ebx\n");
			break;
		case MOD:
			fprintf(objfile, "\tmovl %%eax, %%ebx\n");
			fprintf(objfile, "\tpopl %%eax\n");
			fprintf(objfile, "\tcltd\n");
			fprintf(objfile, "\tidivl %%ebx\n");
			fprintf(objfile, "\tmovl %%edx, %%eax\n");
			break;
		case AND:
			fprintf(objfile, "\tandl (%%esp), %%eax\n");
			fprintf(objfile, "\taddl $4, %%esp\n");
			break;
	}
}

static void apply_oplus(int op)
{
	switch (op) {
		case '+':
			fprintf(objfile, "\taddl (%%esp), %%eax\n");
			fprintf(objfile, "\taddl $4, %%esp\n");
			break;
		case '-':
			fprintf(objfile, "\tmovl %%eax, %%ebx\n");
			fprintf(objfile, "\tpopl %%eax\n");
			fprintf(objfile, "\tsubl %%ebx, %%eax\n");
			break;
	}
}

void smpexpr(void)
{
	int otimesflg = 0, oplusflg = 0;
	int unary = 0;
	char name[MAXSTRLEN+1];

	if (has_error)
		return;

	if (lookahead == '+' || lookahead == '-') {
		unary = lookahead;
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
			strncpy(name, lexeme, MAXSTRLEN);
			name[MAXSTRLEN] = 0;
			match(ID); // ID is associated to a R-value or L-value variable
			if (lookahead == ASGN) {
				// ID := E -> L-value
				match(ASGN);
				expr();
				fprintf(objfile, "\tmovl %%eax, %s\n", name);
			} else {
				fprintf(objfile, "\tmovl %s, %%eax\n", name);
			}
			break;
		case FLT:
			fprintf(objfile, "\tmovl $%s, %%eax\n", lexeme);
			match(FLT);
			break;
		case UINT:
			fprintf(objfile, "\tmovl $%s, %%eax\n", lexeme);
			match(UINT);
			break;
		default:
			match('(');
			expr();
			match(')');
	}

	if (unary == '-') {
		fprintf(objfile, "\tnegl %%eax\n");
		unary = 0;
	} else if (unary == '+') {
		unary = 0;
	}

	if (otimesflg) {
		apply_otimes(otimesflg);
		otimesflg = 0;
	}

	if (isotimes()) {
		fprintf(objfile, "\tpushl %%eax\n");
		otimesflg = lookahead;
		match(lookahead);
		goto _F;
	}

	// term ends here
	if (oplusflg) {
		apply_oplus(oplusflg);
		oplusflg = 0;
	}

	if (lookahead == '+' || lookahead == '-') {
		fprintf(objfile, "\tpushl %%eax\n");
		oplusflg = lookahead;
		match(lookahead);
		goto _T;
	}
}
