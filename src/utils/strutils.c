#include "strutils.h"
#include "charutils.h"

/**
 * Get the first non-space character of a string
 * @param str the given string
 * @return the pointer to the first non-space character of a string
 */
char *getStart(char *str) {
    while (isspace(*str))
        str++;
    return str;
}

/**
 * Get the last non-space character of a string
 * @param str the given string
 * @return the pointer to the last non-space character of a string
 */
char *getEnd(char *str) {
    if (*str == '\0')
        return str;
    for (; *(str+1) != '\0'; str++)
        ; /* find end of string */
    for (; isspace(*str); str--)
        ; /* find first character from end */
    return str;
}

/**
 * Trim a string's surrounding spaces
 * @param str the pointer to the given string
 * @return the length of the string 
 */
int trim(char *str) {
    /* remove leading spaces */
    int i;
    char *start = getStart(str);
    char *end = getEnd(str);
    if (*str == '\0')
        return 0;
    
    if (str != start) {
        for (i = 0; i <= (int)(end - start); i++)
            str[i] = start[i];  /* move back */
    }
    else
        i = (int)(end - start + 1);
    
    /* remove trailing spaces (close the string) */
    str[i] = '\0';
    
    return i;
}

/**
 * Remove the trailing spaces of a string 
 * @param str the given string
 */
void trimEnd(char *str) {
    char *end;
    if (*str != '\0') {
        end = getEnd(str);
        *(end + 1) = '\0';
    }
}
