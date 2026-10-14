//<lexer.h>::
#pragma once

#define MAXSTRLEN 256

extern char lexeme[];

extern void skipspaces(FILE *tape);
extern void skipcomments(FILE *tape);
