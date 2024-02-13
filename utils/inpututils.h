#ifndef INPUTUTILS
#define INPUTUTILS
#include <stdio.h>

#define MAXLINE         81  /* maximum amount of characters in a line including terminating character */

#define getLine_FILE_END    (-1)    /* getLine return value for end of file */
#define getLine_TOO_LONG    (-2)    /* getLine return value for a line that was too long */

int getLine(FILE *fp, char line[], int maxlen);

#endif /* INPUTUTILS */
