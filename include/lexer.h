// Analisador léxico: tokens, lexema e contagem de linhas.
#pragma once

#include <stdio.h>

#define MAXSTRLEN 256

extern char lexeme[];
extern int line;

void skipspaces(FILE *tape);
void skipcomments(FILE *tape);
int gettoken(FILE *src);
