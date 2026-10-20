// Implementação da pilha de avaliação e do acumulador.

#include <vmachine.h>

double acc = 0;
double stack[MAXSTACKSIZE];
int sp = -1;

void push(void) /* empilha o acumulador */
{
	sp++;
	stack[sp] = acc;
}

double pop(void) /* desempilha para o chamador */
{
	double aux = stack[sp];
	sp--;
	return aux;
}
