#include "inpututils.h"
#include "charutils.h"
#include "strutils.h"
#include "logger.h"

/* getLine: get the next input line from given file
 * fp - the file to read from
 * line - the char* to read the next line into. the closing '\n' or EOF are replaced with a '\0'
 * maxlen - the maximum amount of characters to read
 * len - the length of the actual line read
 * Returns:
 * - If the file has ended, returns getLine_FILE_END
 * - If the line read was too long (longer than maxlen), returns getLine_TOO_LONG.
 * - If the line read was empty, returns getLine_EMPTY
 * - If the line read was a commented line, returns getLine_COMMENT
 * - Else (the line is valid), returns the amount of characters read */
enum getLineStatus getLine(FILE *fp, char line[], unsigned int maxlen, int *len) {
    int c;
    maxlen -= 1;     /* account for '\0' termination */

    if ((c = getc(fp)) == COMMENT_CHAR) {
        for (*len = 1; *len < maxlen && (c = getc(fp)) != '\n' && c != EOF; (*len)++)
            ;   /* skip to end of line to verify length */

        if (*len == maxlen) {
            line[*len] = '\0';
            if ((c = getc(fp) != '\n') && c != EOF) {
                while ((c = getc(fp)) != '\n' && c != EOF)
                    ;   /* skip to end of line */
                return getLine_TOO_LONG;
            }
        }

        return getLine_COMMENT;
    }
    else
        ungetc(c, fp);


    for (*len = 0; *len < maxlen && (c = getc(fp)) != '\n' && c != EOF; (*len)++)
        line[*len] = (char)c;
    
    
    line[*len] = '\0';
    if (*len == maxlen) {
        if ((c = getc(fp)) != '\n' && c != EOF) {
            while ((c = getc(fp)) != '\n' && c != EOF)
                ; /* skip to end of line */
            return getLine_TOO_LONG;
        }
    }
    else {
        if (*len == 0 && c == EOF)
            return getLine_FILE_END;
    }

    return getLine_VALID;
}

/**
 * Get the next non-empty and not commented line (trimmed)
 * Error handling is the same as getLine (except getLine_COMMENT, which never occurres)
 * @param fp the file to read from
 * @param line the line to read into
 * @param maxlen the maximum line size
 * @return the length of the trimmed line, or an error if one occurred
 */
enum getLineStatus getNextLine(FILE *fp, char line[], unsigned int maxlen, int *len) {
    int err;
    
    while ((err = getLine(fp, line, maxlen, len)) == getLine_VALID || err == getLine_COMMENT) {
        if (err != getLine_COMMENT && (*len = trim(line)) > 0)
            return getLine_VALID;
        /* else, line is empty or a comment - read the next line */
    }
    
    return err;
}
