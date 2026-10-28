// Códigos de tokens compostos (os operadores usam o próprio ASCII).
#pragma once

enum {
	ID = 1024,
	DEC,
	FLT,
	ASGN, // := right-associative assignment
	LEQ,
	NEQ,
	GEQ,
};
