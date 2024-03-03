#ifndef STRUTILS
#define STRUTILS

#include <string.h>

char *getStart(char *str);
char *getStrEnd(char *str);
int trim(char *str);
char *getTokEnd(char *tok);
char *getNextToken(char *tok);
int tokcmp(char *str1, char *str2);
char *getFirstOrEnd(char *str, char c);
int tryParseNumber(char *str, double *number);

#endif /* STRUTILS */
