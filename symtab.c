// Tabela de símbolos: busca e inserção de nomes.

#include <string.h>
#include <lexer.h>
#include <symtab.h>
#include <stdio.h>
#include <stdlib.h>

SYMTAB symtab[SYMTABSIZE];
int symtab_nextentry = 0;
double vmem[SYMTABSIZE];

int symtab_lookup(const char *varname)
{
	int i;
	for (i = 0; i < symtab_nextentry; i++) {
		if (strcmp(symtab[i].name, varname) == 0) {
			return i;
		}
	}
	return -1; // varname not found
}

int symtab_append(const char *varname)
{
	int found;
	if (symtab_nextentry < SYMTABSIZE) {
		strcpy(symtab[found = symtab_nextentry++].name, varname);
	} else {
		fprintf(stderr, "Erro fatal: limite da tabela de simbolos excedido.\n");
		return -1; // symtab overflow
	}
	return found;
}

void store(char *name, double val)
{
	int i = symtab_lookup(name);
	if (i < 0)
		i = symtab_append(name);
	if (i < 0)
		return;
	vmem[i] = val;
}

double recall(char *name)
{
	int i = symtab_lookup(name);
	if (i < 0)
		i = symtab_append(name);
	if (i < 0)
		return 0;
	return vmem[i];
}
