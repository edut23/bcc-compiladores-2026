// Códigos de tokens compostos (os operadores usam o próprio ASCII).
#pragma once

enum {
	ID = 1024,
	UINT,
	FLT,
	ASGN, // := right-associative assignment
	LEQ,
	NEQ,
	GEQ,
};
