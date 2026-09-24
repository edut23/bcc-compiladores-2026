#pragma once

extern FILE *source; // to be defined at main.c
extern FILE *output;

extern int lookahead;
extern void E(void);
extern void T(void);
extern void R(void);
extern void Q(void);
extern void F(void);
extern void match(int required);
extern int line;
extern int gettoken(FILE *); // defined at lexer.c