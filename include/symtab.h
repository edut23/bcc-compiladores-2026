// Tabela de símbolos: nomes, tipos e deslocamentos.
#pragma once

#include <lexer.h>

#define SYMTABSIZE 1024

typedef struct _symtab_ {
	char name[MAXSTRLEN+1];
	int type;
	char offset[MAXSTRLEN+1];
	// paddle for future use
	char paddle[SYMTABSIZE];
} SYMTAB;

extern SYMTAB symtab[];
extern int symtab_nextentry;

extern int symtab_lookup(const char *varname);
extern int symtab_append(const char *varname);
extern void symtab_release(void);
extern void symtab_batch(int type);
extern void store(char *name, double val);
extern double recall(char *name);

extern int lexlevel;
#define MAX_LEXICAL_LEVEL 100
extern int upper_bound[];
