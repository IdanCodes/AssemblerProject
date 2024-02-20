#ifndef INPUTUTILS
#define INPUTUTILS
#include <stdio.h>

#define MAXLINE         20  /* maximum amount of characters in a line including terminating character */

/* represents return/error statuses from getLine and similar methods */
enum getLineStatus {
    getLine_VALID = -5,
    getLine_FILE_END,
    getLine_TOO_LONG,
    getLine_COMMENT
};

enum getLineStatus getLine(FILE *fp, char line[], unsigned int maxlen, int *len);
enum getLineStatus getNextLine(FILE *fp, char line[], unsigned int maxlen, int *len);

#endif /* INPUTUTILS */
