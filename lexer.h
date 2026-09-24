//<lexer.h>::
#pragma once

#define MAXSTRLEN 256

extern char lexeme[];

void skipspaces(FILE *tape);
void skipcomments(FILE *tape);
