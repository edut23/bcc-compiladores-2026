// Parser LL(1) e laço do interpretador MyBC.
#pragma once

#include <stdio.h>

extern FILE *source;
extern FILE *output;

extern int lookahead;
extern int has_error;

void expr(void);
void match(int required);
void mybc(void);
