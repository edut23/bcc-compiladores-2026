#pragma once

#include <stdio.h>

extern void program(void);
extern int expr(int inherited);
extern int smpexpr(int inherited);
extern void idlist(void);
extern void block(void);
extern int type(void);
extern void parmlist(void);
extern void stmtlist(void);
extern void stmt(void);
extern void arglist(void);
extern int loopcount;
extern int lexlevel;
#define MAX_LEXICAL_LEVEL 100
extern int upper_bound[];

extern FILE *targetfile;
extern FILE *objfile; // object code

extern int lookahead;
extern int has_error;

void match(int required);
