#pragma once

#include <stdio.h>

extern void program(void);
extern int expr(int inherited);
extern int smpexpr(int acctype);
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

/* type convention:
 * void:    1
 * boolean: 2
 * int32:   3
 * int64:   4
 * flt32:   5
 * flt64:   6
 */
#define TYPE_VOID    1
#define TYPE_BOOLEAN 2
#define TYPE_INT32   3
#define TYPE_INT64   4
#define TYPE_FLT32   5
#define TYPE_FLT64   6

int iscompat(int acctype, int newtype);
int isrelop(void);

void match(int required);
