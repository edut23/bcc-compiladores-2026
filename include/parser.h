#pragma once

#include <stdio.h>

extern void program(void);
extern void expr(void);
extern void smpexpr(void);
extern void idlist(void);
extern void block(void);
extern void type(void);
extern void parmlist(void);
extern void stmtlist(void);
extern void stmt(void);
extern void arglist(void);
extern int loopcount;

extern FILE *source;
extern FILE *output; // object code

extern int lookahead;
extern int has_error;

void match(int required);
