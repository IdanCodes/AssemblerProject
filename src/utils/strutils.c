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
char *getStrEnd(char *str) {
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
    char *end = getStrEnd(str);
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
 * Get the last non-space character of the first token (first token in the string)
 * @param tok the given token
 * @return the desired pointer
 */
char *getTokEnd(char *tok) {
    if (isspace(*tok) || *tok == '\0')
        return tok;
    for (; !isspace(*tok) && *tok != '\0'; tok++)
        ;   /* wait for a space/termination */
    return tok-1;
}

/**
 * Get the next token in the string. If the given pointer points to the last character of a string, it will be returned.
 * @param tok the given token
 * @return the pointer to the next token in the string
 */
char *getNextToken(char *tok) {
    return *tok == '\0' ? tok : getStart(getTokEnd(tok) + 1);
}

/**
 * Compare the first tokens of the given strings
 * @param str1 a given string
 * @param str2 a given string
 * @return the result of comparison of the two first tokens of the given strings
 */
int tokcmp(char *str1, char *str2) {
    unsigned int i, len, len1, len2;
    
    len = ((len1 = (getTokEnd(str1) - str1)) < (len2 = (getTokEnd(str2) - str2)) ? len1 : len2) + 1;
    
    for (i = 0; i < len; i++) {
        if (str1[i] != str2[i])
            return str1[i] - str2[i];
    }
    
    return ((str1[len] == '\0' || str1[len] == ' ') && (str2[len] == '\0' || str2[len] == ' ')) ? 0
            : (str1[len] == '\0' || str1[len] == ' ') ? -1 : 1;
}

/**
 * Get the first occurrence of a character in a string, if it doesn't exist returns the end of the string
 * @param str The string to search in
 * @param c The character to look for
 * @return The first occurrence of the requested character or the address of the string's end
 */
char *getFirstOrEnd(char *str, char c) {
    for (; *str != c && *str != '\0'; str++)
        ;   /* wait for condition to be false */
    return str;
}

/**
 * tries to parse a string's first token to an integer
 * @param str The string to parse
 * @param number A pointer to the parsed number
 * @return Whether the parsing was successful (whether the token is a valid integer)
 */
int tryParseToken(char *str, int *number) {
    int sign, zeroPrefix, result;
    char *end;

    end = getTokEnd(str) + 1; /* character after last one */
    sign = 1;
    result = 0;

    if (*str == '-') {
        sign = -1;
        str++;
    }
    else if (*str == '+')
        str++;

    /* skip 0 prefix */
    zeroPrefix = (*str == '0');
    while (*str == '0')
        str++;

    if (str == end && zeroPrefix && sign > 0) /* don't accept "-0" */ {
        *number = 0;
        return 1;   /* just a string full of zeroes */
    }

    if (str == end && !zeroPrefix)  /* if we've reached the end of the token (and there were no prefixing zeros) */
        return 0;   /* not a number */

    /* read whole part */
    for (; str < end; str++) {
        if (!isdigit(*str))    /* not a digit nor a '.' */
            return 0;

        result *= 10;
        result += *str - '0';   /* "un-ascii-fy" the char */
    }

    *number = result * sign;
    return 1;
}

/**
 * Get the end of the operand - address of the first space (isspace), termination ('\0') or comma (',')
 * @param str the given string to search in
 * @return address of the desired character in the string
 */
char *getEndOfOperand(char *str) {
    if (*str == '\0' || isspace(*str) || *str == ',')
        return str;
    
    for (str++; *str != '\0' && !isspace(*str) && *str != ','; str++)
        ;   /* wait until the operand ends */
    
    return str;
}

/**
 * Check if the given char pointer is inside the token
 * @param tok the token to search inside (the start of it)
 * @param pc the address to search for
 * @return 1 if the given char pointer is inside the token, 0 otherwise.
 */
int isInTok(char *tok, char *pc) {
    return tok <= pc && getTokEnd(tok) >= pc;
}
