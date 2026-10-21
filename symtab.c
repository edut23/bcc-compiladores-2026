// Memória de variáveis: busca ou insere o nome e guarda o valor em vmem.

#include <string.h>
#include <lexer.h>
#include <symtab.h>
#include <stdio.h>  
#include <stdlib.h> 

/*
typedef struct _symtab_ {
	char name[MAXSTRLEN+1];
	int type;
	char offset[MAXSTRLEN+1];
} SYMTAB;
*/

double vmem[SYMTABSIZE];
SYMTAB symtab[SYMTABSIZE];
int symtab_next = 0;

int symtab_lookup(char *name) /* índice de `name`, inserindo se ainda não existir */
{
	int i;
	for (i = 0; i < symtab_next; i++)
		if (strcmp(symtab[i], name) == 0) return i;

	if (symtab_next >= SYMTABSIZE) {
		fprintf(stderr, "Erro fatal: limite da tabela de simbolos excedido.\n");
		exit(1);
	}

	strcpy(symtab[symtab_next], name);
	return symtab_next++;
}

int symtab_append(const char *name)
{
	if (symtab_next >= SYMTABSIZE) {
		fprintf(stderr, "Erro fatal: limite da tabela de simbolos excedido.\n");
		exit(1);
	}
	strcpy(symtab[symtab_next].name, name);
	symtab[symtab_next].type = type;
	symtab[symtab_next].offset = offset;
	return symtab_next++;
}

void store(char *name, double val) /* grava `val` na variável `name` */
{
	vmem[lookup(name)] = val;
}

double recall(char *name) /* devolve o valor de `name` */
{
	return vmem[lookup(name)];
}
