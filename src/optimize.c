#include "optimize.h"
#include "options.h"
#include <string.h>
#include <stdlib.h>

#define OPPOSITE(n) (n=='+'?'-':n=='-'?'+':n=='<'?'>':n=='>'?'<':0)
#define VALID_CHAR(n) (n=='+'||n=='-'||n=='<'||n=='>'||n=='['||n==']'\
						 || n==',' || n=='.' )

void optimize(char *code)
{
	unsigned int i, j;
	/* We're calling strlen here because it is brought up more than once in the code below */
	unsigned int len = strlen(code);

	/* Return if code is empty string */
	if (len == 0)
		return;

	/* Loop through code and add the OK characters */
	for (i = 0, j = 0; i < len; i++) {
		/* Add pairless valid characters */
		if (i < (len - 2) && code[i] == OPPOSITE(code[i + 1])) {
			/* Skip next character */
			i += 1;
		} else if (VALID_CHAR(code[i])) {
			/* Add character */
			code[j] = code[i];
			j++;
		}
	}

	/* Add null terminator */
	code[j] = '\0';

	/* Recurse */
	if (len != j) {
		optimize(code);
	}
}
