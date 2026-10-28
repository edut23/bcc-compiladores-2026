#pragma once

extern char *keywordlist[];
extern int keylistLen;
extern long unsigned iskeyword(const char *);

/* Começa longe de ASCII (0–127) e dos tokens de tokens.h (1024+).
 * BEGIN não pode ser 0: isID usa `if ((token = iskeyword(lexeme)))`. */
enum _KEYWORD_TOKENS {
	BEGIN = 2048,
	IF,
	THEN,
	ELSE,
	WHILE,
	DO,
	REPEAT,
	UNTIL,
	NOT,
	OR,
	DIV,
	MOD,
	AND,
	TRUE,
	FALSE,
	PROGRAM,
	PROCEDURE,
	FUNCTION,
	VAR,
	INTEGER,
	REAL,
	BOOLEAN,
	END,
};
