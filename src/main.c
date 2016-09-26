/*
Brainfuck to MIPS Compiler
Copyright (C) 2016 Dagan Martinez

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "compile.h"
#include "options.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define VERSION "bfmips 0.1.0"
#define USAGE "Usage: bfmips [options] [infile | -i cmd] [-o outfile]\n"

int main(int argc, char *argv[])
{
	/* File Variables */
	char *ifname = "";
	char *ofname = "a.out";
	int ifname_set = 0;
	int ofname_set = 0;

	/* Prepare options variables */
	int options = 0;
	SET_OPTIONS_VARIABLE(options);

	/* Parse arguments */
	if (argc <= 1) {
		/* No arguments */
		fprintf(stderr, "bfmips: no arguments\n" USAGE);
		return EXIT_FAILURE;
	} else {
		int i, j;

		/* Look through arguments */
		for (i = 1; i < argc; i++) {
			if ((argv[i])[0] == '-') {
				/* check for -o */
				if (argv[i][1] == 'o'
				    && argv[i][2] == '\0') {
					if (i + 1 < argc
					    && argv[i + 1][0] != '-') {
						ofname =
						    malloc(sizeof(char) *
							   (1 +
							    strlen(argv
								   [i +
								    1])));
						for (j = 0;
						     (argv[i + 1])[j] != 0;
						     j++)
							ofname[j] =
							    argv[i + 1][j];
						ofname[j] = 0;
						ofname_set = 1;
					} else {
						fprintf(stderr,
							"bfmips: no output file\n");
				exit(EXIT_FAILURE);	}
					i++;
					continue;
				}

				/* Check for valid options */
				for (j = 1; argv[i][j] != '\0'; j++) {
					int new_option =
					    GET_OPTION(argv[i][j]);
					if (new_option == OPT_NONE) {
						/* Check if "--h[elp]" or "--v[ersion]" */
						if (argv[i][j] == '-'
						    && (argv[i][j + 1] ==
							'h'
							|| argv[i][j +
								   1] ==
							'v')) {
							ADD_OPTION
							    (GET_OPTION
							     (argv[i]
							      [j + 1]));
							break;
						} else {
							/* Option is invalid, abort */
							fprintf(stderr,
								"bf: no option `%c`\n",
								argv[i]
								[j]);
							return
							    EXIT_FAILURE;
						}
					}

					ADD_OPTION(new_option);
				}
			}

			else {
				/* Set input file name */
				ifname =
				    malloc(sizeof(char) *
					   (1 + strlen(argv[i])));
				for (j = 0; (argv[i])[j] != '\0'; j++)
					ifname[j] = (argv[i])[j];
				ifname[j] = '\0';
				ifname_set = 1;
			}
		}
	}
	if (HAS_OPTION(OPT_HELP)) {
		printf(USAGE "\n"
		       "Options:\n"
		       "\t-h\tPrint help message\n"
		       "\t-i cmd\tPass string as code\n"
		       "\t-t\tDisplay compile time of program\n"
		       "\t-v\tDisplay version\n");
	} else if (HAS_OPTION(OPT_VER)) {
		/* Print out version number */
		printf(VERSION "\n");
	} else if (HAS_OPTION(OPT_STRING)) {
		/* Interpret from argument */
		if (ifname_set) {
			FILE *ofp = fopen(ofname, "w");
			compile(ifname, ofp, options);
			fclose(ofp);
		} else {
			fprintf(stderr, "bfmips: no command\n" USAGE);
			return EXIT_FAILURE;
		}
	} else if (ifname_set) {
		/* Open file for reading */
		FILE *fp;
		int fp_size;
		char *fp_contents;
		fp = fopen(ifname, "rb");
		/* Check for errors */
		if (fp == NULL) {
			fprintf(stderr,
				"bf: input file \"%s\" not found\n", ifname);
			free(ifname);
			return EXIT_FAILURE;
		}

		/* Get size */
		fseek(fp, 0, SEEK_END);
		fp_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		/* Read file */
		fp_contents = malloc((sizeof(char) + 1) * fp_size);
		fp_contents[fp_size] = '\0';	/* Terminate with null */
		fread(fp_contents, 1, fp_size, fp);
		fclose(fp);
		/* Compile source code */
		FILE *ofp = fopen(ofname, "w");
		compile(fp_contents, ofp, options);
		/* Free everything */
		fclose(ofp);
		free(fp_contents);
	} else {
		fprintf(stderr, "bf: no input files\n" USAGE);
		return EXIT_FAILURE;
	}

	/* Finish */
	if (ifname_set)
		free(ifname);
	if (ofname_set)
		free(ofname);
	return EXIT_SUCCESS;
}
