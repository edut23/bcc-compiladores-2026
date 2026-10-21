/* keywords.c */

#include <string.h>
#include <keywords.h>

char *keywordList[] = {
    "begin",
    "if",
    "then",
    "else",
    "while",
    "do",
    "repeat",
    "until",
    "not",
    "or",
    "div",
    "mod",
    "and",
    "true",
    "false",
    "end",
};

/* sizeof só funciona aqui, onde o array está definido.
 * Se keywordList decaísse para ponteiro, isso daria 1. */
int keylistLen = (int)(sizeof(keywordList) / sizeof(keywordList[0]));

int isKeyword(const char *identifier) {
    for (int i = 0; i < keylistLen; i++) {
        if (strcmp(identifier, keywordList[i]) == 0) {
            return (long unsigned) keywordList[i];
        }
    }
    return 0;
}