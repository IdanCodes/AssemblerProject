#include "inpututils.h"
#include "strutils.h"
#include "charutils.h"

/**
 * Get the next input line from a given file
 * @param fp the file to read from
 * @param line the string to read the next line into. closing '\n' or EOF are replaced with a '\0' (assuming it is big enough to store the line)
 * @param maxlen the maximum amount of characters to read
 * @param len the length of the actual line read
 * @return an enum getLineStatus byte according to the input
 */
enum getLineStatus getLine(FILE *fp, char line[], unsigned int maxlen) {
    int c, len;
    
    for (len = 0; len < maxlen && (c = getc(fp)) != '\n' && c != EOF; (len)++)
        line[len] = (char)c;
    
    line[len] = '\0';
    if (len == maxlen) {
        if ((c = getc(fp)) != '\n' && c != EOF) {
            while ((c = getc(fp)) != '\n' && c != EOF)
                ;   /* skip to end of line */
            return getLine_TOO_LONG;
        }
    }
    else if (len == 0 && c == EOF)
            return getLine_FILE_END;
    
    return getLine_VALID;
}

/**
 * Get the non-empty, non-commented input line from a given file
 * @param fp the file to read from
 * @param line the string to read the next line into. closing '\n' or EOF are replaced with a '\0' (assuming it is big enough to store the line)
 * @param skipLong whether to skip lines that are too long
 * @param maxlen the maximum amount of characters to read
 * @param len the length of the actual line read
 * @return the line status of the line stored inside the char array 'line'.
 * @return if the file ended, returns getLine_FILE_END. otherwise returns the number of lines skipped.
 */
enum getLineStatus getNextLine(FILE *fp, char line[], int skipLong, unsigned int maxlen, unsigned int *skippedLines) {
    enum getLineStatus lineErr;

    for (*skippedLines = 1; (lineErr = getLine(fp, line, maxlen)) != getLine_FILE_END; (*skippedLines)++) {
        if (lineErr == getLine_TOO_LONG) {
            if (!skipLong)
                return getLine_TOO_LONG;
        }
        else if (trim(line) != 0 && line[0] != COMMENT_CHAR)
            return getLine_VALID;   /* not empty nor comment */
    }

    return getLine_FILE_END;
}
