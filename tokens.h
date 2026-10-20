// Códigos de tokens compostos (os operadores usam o próprio ASCII).
#pragma once

/*
#define ID 1024
#define DEC 1025
*/
#define NUM DEC // definicao temoraria

enum {
	ID = 1024,
	DEC,
	OCT,
	HEX,
	ASGN, // := right-associative assignment
};
