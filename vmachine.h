// Máquina virtual mínima: acumulador e pilha de operandos.
#pragma once

#define MAXSTACKSIZE 1024

extern double acc;
extern int sp;

void push(void);
double pop(void);
