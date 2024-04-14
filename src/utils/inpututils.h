#ifndef INPUTUTILS
#define INPUTUTILS
#include <stdio.h>

#define MAXLINE         81  /* maximum amount of characters in a line (not including terminating character) */

/* represents return/error statuses from getLine and similar methods */
enum getLineStatus {
    getLine_VALID = -3,
    getLine_FILE_END,
    getLine_TOO_LONG
};

enum getLineStatus getLine(FILE *fp, char line[], unsigned int maxlen);
enum getLineStatus getNextLine(FILE *fp, char line[], int skipLong, unsigned int maxlen, unsigned int *skippedLines);

#endif /* INPUTUTILS */
