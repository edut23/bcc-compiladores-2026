// Implementação da pilha de avaliação e do acumulador.

#include <vmachine.h>
#include <stdio.h>  
#include <stdlib.h> 

double acc = 0;
double stack[MAXSTACKSIZE];
int sp = -1;

void push(void) /* empilha o acumulador */
{
	if (sp >= MAXSTACKSIZE - 1) {
		fprintf(stderr, "Erro fatal: stack overflow\n");
		exit(1);
	}
	sp++;
	stack[sp] = acc;
}

double pop(void) /* desempilha para o chamador */
{
	if (sp < 0) {
		fprintf(stderr, "Erro fatal: stack underflow\n");
		exit(1);
	}
	double aux = stack[sp];
	sp--;
	return aux;
}
