#include <math.h>
#include "strutils.h"
#include "charutils.h"
#include "logger.h"

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
 * Get the next token in the string
 * @param tok the given token
 * @return the pointer to the next token in the string
 */
char *getNextToken(char *tok) {
    return getStart(getTokEnd(tok) + 1);
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
 * @param str 
 * @param c 
 * @return The first occurrence of the requested character or the address of the string's end
 */
char *getFirstOrEnd(char *str, char c) {
    for (; *str != c && *str != '\0'; str++)
        ;   /* wait for condition to be false */
    return str;
}

/* tryParseNumber: tries to parse a string's first token to double.
 * returns whether the parsing was successful. (whether the token is a valid double) */
int tryParseNumber(char *str, double *number) {
    int sign, zeroPrefix;
    char *end;
    double power, result;

    end = getTokEnd(str) + 1; /* character after last one */
    sign = 1;
    result = 0;

    if (*str == '-') {
        sign = -1;
        str++;
    }

    /* skip 0 prefix */
    zeroPrefix = (*str == '0');
    while (*str == '0')
        str++;

    if (str == end && zeroPrefix && sign > 0) /* don't accept "-0" */
        return 1;   /* just a string full of zeroes */

    if ((str == end && !zeroPrefix) ||   /* if we've reached the end of the token (and there were no prefixing zeros) */
        (!isdigit(*str) && (!zeroPrefix || *str != '.')) ||   /* if the character is not a digit and not a valid period */
        (*str == '.' && str + 1 == end))  /* if the character is a period and the last character */
        return 0; /* not a number */

    /* read whole part */
    for (; str < end && *str != '.'; str++) {
        if (!isdigit(*str))    /* not a digit nor a '.' */
            return 0;

        result *= 10;
        result += *str - '0';   /* "un-ascii-fy" the char */
    }

    if ((str++) < end) {
        /* read decimal part */
        for (power = 1; str < end; str++) {
            if (!isdigit(*str))
                return 0;

            result += (*str - '0') * pow(10, -(power++));
        }
    }

    *number = result * sign;
    return 1;
}
