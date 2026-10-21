// Tabela de símbolos: nomes de variáveis e seus valores.
#pragma once

#define SYMTABSIZE 1024

typedef struct _symtab_ {
	char name[MAXSTRLEN+1];
	int type;
	char offset[MAXSTRLEN+1];
} SYMTAB;

extern SYMTAB symtab[SYMTABSIZE];
extern int symtab_next;

int symtab_lookup(char *name);
int symtab_append(const char *name);
void store(char *name, double val);
double recall(char *name);
