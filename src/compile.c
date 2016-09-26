#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compile.h"
#include "timer.h"
#include "options.h"

/* Brainfuck cell */
#define CELL unsigned char

/* ctype codes */
#define BF_SHIFT '>'
#define BF_ADD '+'
#define BF_SKIP 's'
#define BF_GOTO 'g'
#define BF_PUT '.'
#define BF_GET ','
#define BF_DEBUG '~'
#define BF_END 'E'
struct Token {
	char ctype;		//command type
	int value;
};
/* Convert code into list of commands 
 * The idea here is that this is faster to run
* than interpreting the ['s and ]'s as you go along*/
static void tokenize(struct Token *commands, const char *code)
{
	int count = 0;		/* How many command objects have we put in the 'commands' list */
	int skip_max = 10;	/* Size of skip queque */
	int *skip_queue = calloc(skip_max, sizeof(int));
	int skips = 0;		/* Where we are in the skip queue */
	for (int i = 0; code[i] != '\0'; i++) {
		if (code[i] == '[') {
			/* Record where a '[' would go to */
			commands[count].ctype = BF_SKIP;
			skip_queue[skips] = count;
			count++;
			skips++;
			/*Reallocate skip_queue */
			if (skips >= skip_max) {
				int *buffer =
				    calloc(skip_max, sizeof(int));
				for (int j = 0; j < skip_max; j++) {
					buffer[j] = skip_queue[j];
				}
				free(skip_queue);
				skip_queue =
				    calloc(skip_max + 1, sizeof(int));
				for (int j = 0; j < skip_max; j++) {
					skip_queue[j] = buffer[j];
				}
				free(buffer);
				skip_max++;
			}
		} else if (code[i] == ']') {
			/* Record where a ']'would go back to */
			skips--;
			commands[count].ctype = BF_GOTO;
			commands[count].value = skip_queue[skips];
			commands[skip_queue[skips]].value = count;
			count++;
		} else if (code[i] == '<' || code[i] == '>') {
			/* Add up all <'s and >'s */
			commands[count].ctype = BF_SHIFT;
			commands[count].value = code[i] == '<' ? -1 : 1;
			while (code[i + 1] == '<' || code[i + 1] == '>') {
				commands[count].value +=
				    code[i + 1] == '<' ? -1 : 1;
				i++;
			}
			/* Only add code if it's not dead */
			if (commands[count].value != 0) {
				count++;
			}
		} else if (code[i] == '+' || code[i] == '-') {
			commands[count].ctype = BF_ADD;
			commands[count].value = code[i] == '-' ? -1 : 1;
			while (code[i + 1] == '-' || code[i + 1] == '+') {
				commands[count].value +=
				    code[i + 1] == '-' ? -1 : 1;
				i++;
			}
			/* Only add code if it's not dead */
			if (commands[count].value != 0) {
				count++;
			}
		} else if (code[i] == '.') {
			/* Note: Value doesn't do anything. Consecutive '.''s'
			 *                      are just consecutive commands. I don't think this has a performance hit */
			commands[count].ctype = BF_PUT;
			commands[count].value = 1;
			count++;
		} else if (code[i] == ',') {
			/* See above */
			commands[count].ctype = BF_GET;
			count++;
		} else if (code[i] == '~') {
			commands[count].ctype = BF_DEBUG;
			count++;
		}

	}
	/* This object tells interpreter to stop running */
	commands[count].ctype = BF_END;
	free(skip_queue);
}

/* Interpret commands */
void compile(const char *code, int options)
{
	/* Create list of commands from code */
	struct Token *commands;
	commands = malloc(sizeof(struct Token) * (strlen(code) + 2));
	tokenize(commands, code);

	int tape_size = 1;
	CELL *tape = calloc(tape_size, sizeof(CELL));

	/* Set up options */
	SET_OPTIONS_VARIABLE(options);

	/* Record time if -t */
	START_TIMER;

	FILE *fp = stdout;
	fprintf(fp,
		".text\n\taddi $s0, $zero, 0x10010020\nadd $t0, $zero, $zero\n\tli $v0, 11\n");
	/* Go through command objects */
	for (int i = 0; commands[i].ctype != BF_END; i++) {
		switch (commands[i].ctype) {
		case BF_ADD:
			fprintf(stdout,
				"\taddi $t0, $t0, %i\n\tsb $t0, 0($s0)\n",
				commands[i].value);
			break;
		case BF_SHIFT:
			fprintf(stdout,
				"\taddi $s0, $s0, %i\n\tlb $t0, 0($s0)\n",
				commands[i].value);
			break;
		case BF_PUT:
			fprintf(fp, "\tadd $a0, $zero, $t0\n\tsyscall\n");
			break;
		case BF_GET:
			fprintf(fp,
				"\tli $v0, 12\n\tsyscall\n\tadd $t0, $zero, $v0\n\tsb $t0, 0($s0)\n\tli $v0, 11\n");
			break;
		case BF_SKIP:
			if (commands[i + 1].ctype == BF_ADD
			    && commands[i + 2].ctype == BF_GOTO) {
				fprintf(stdout,
					"\tadd $t0, $zero, $zero\n\tsb $t0, 0($s0)\n");
				i += 2;
			} else {
				fprintf(stdout,
					"start_%i:\n\tbeq $t0, $zero, end_%i\n",
					i, commands[i].value);
			}
			break;
		case BF_GOTO:
			fprintf(stdout, "\tj start_%i\nend_%i:\n",
				commands[i].value, i);
			break;
		}
		//printf("{%c:%i}\n",commands[i].ctype,commands[i].value);
	}

	if (HAS_OPTION(OPT_TIMER)) {
		STOP_TIMER;
		PRINT_TIMER;
	}
	free(commands);
	free(tape);
}
