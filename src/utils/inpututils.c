#include "inpututils.h"
#include "strutils.h"
#include "charutils.h"

/**
 * Get the next input line from a given file
 * @param fp the file to read from
 * @param line the string to read the next line into. closing '\n' or EOF are replaced with a '\0' (assuming it is big enough to store the line)
 * @param maxlen the maximum amount of characters to read
 * @param len the length of the actual line read
 * @return an enum getLineStatus value according to the input
 */
enum getLineStatus getLine(FILE *fp, char line[], unsigned int maxlen, int *len) {
    int c;
    
    for (*len = 0; *len < maxlen && (c = getc(fp)) != '\n' && c != EOF; (*len)++)
        line[*len] = (char)c;
    
    line[*len] = '\0';
    if (*len == maxlen) {
        if ((c = getc(fp)) != '\n' && c != EOF) {
            while ((c = getc(fp)) != '\n' && c != EOF)
                ;   /* skip to end of line */
            return getLine_TOO_LONG;
        }
    }
    else if (*len == 0 && c == EOF)
            return getLine_FILE_END;
    
    return getLine_VALID;
}

/**
 * Get the next valid length, non-empty, non-commented input line from a given file
 * @param fp the file to read from
 * @param line the string to read the next line into. closing '\n' or EOF are replaced with a '\0' (assuming it is big enough to store the line)
 * @param maxlen the maximum amount of characters to read
 * @param len the length of the actual line read
 * @return if the file ended, returns getLine_FILE_END. otherwise returns the number of lines skipped.
 */
int getNextLine(FILE *fp, char line[], unsigned int maxlen, int *len) {
    enum getLineStatus lineErr;
    int skippedLines;

    for (skippedLines = 1; (lineErr = getLine(fp, line, maxlen, len)) != getLine_FILE_END; skippedLines++) {
        if (lineErr != getLine_TOO_LONG && (*len = trim(line)) != 0 && line[0] != COMMENT_CHAR)
            return skippedLines;   /* not empty nor comment */
    }

    return getLine_FILE_END;
}
