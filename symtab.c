// Memória de variáveis: busca ou insere o nome e guarda o valor em vmem.

#include <string.h>
#include <lexer.h>
#include <symtab.h>
#include <stdio.h>  
#include <stdlib.h> 

#define SYMTABSIZE 1024

double vmem[SYMTABSIZE];
char symtab[SYMTABSIZE][MAXSTRLEN+1];
int symtab_next = 0;

int lookup(char *name) /* índice de `name`, inserindo se ainda não existir */
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

void store(char *name, double val) /* grava `val` na variável `name` */
{
	vmem[lookup(name)] = val;
}

double recall(char *name) /* devolve o valor de `name` */
{
	return vmem[lookup(name)];
}
