#ifndef HEADER_OPTIONS
#define HEADER_OPTIONS 1
	/* Define __option_var__ as pointer to the variable declared as
	   the option variable */
#define SET_OPTIONS_VARIABLE(a) int* __option_var__=&a;

	/* return 'n' from options if exists */
#define HAS_OPTION(n) ((int)((*__option_var__)&n))

	/* Add 'n' to options */
#define ADD_OPTION(n) (*__option_var__|=n);


	/* List of possible options 
	   Values are by powers of 2 */
#define OPT_NONE (0)		/* Empty option */
#define OPT_HELP (1<<1)		/* Display help message (do not interpret) */
#define OPT_VER (1<<2)		/* Show version number */
#define OPT_STRING (1<<4)	/* Lets argument string run as code instead of file */
#define OPT_TIMER (1<<5)	/* Displays the run time of a program */

	/* Get option from option code */
#define GET_OPTION(n) (\
	n=='h'?OPT_HELP:\
	n=='v'?OPT_VER:\
	n=='i'?OPT_STRING:\
	n=='t'?OPT_TIMER:\
	OPT_NONE)
#endif
