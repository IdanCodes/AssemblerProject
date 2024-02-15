#include "inpututils.h"
#include "charutils.h"

/* getLine: get the next input line from given file.
 * ignores lines filled with spaces/tabs and commented lines.
 * fp - the file to read from.
 * line - the char* to read the next line into. the closing '\n' or EOF are replaced with a '\0'.
 * maxlen - the maximum amount of characters to read.
 * Returns:
 * - If the file has ended, returns getLine_FILE_END.
 * - If the line read was too long (longer than maxlen), returns getLine_TOO_LONG.
 * - Else (the line is valid), returns the amount of characters read */
int getLine(FILE *fp, char line[], int maxlen) {
    int i, c;

    if ((c = getc(fp)) == COMMENT_CHAR) {
        for (i = 1; i < maxlen && (c = getc(fp)) != '\n' && c != EOF; i++)
            ;   /* skip to end of line */

        if (i == maxlen) {
            if ((c = getc(fp) != '\n') && c != EOF) {
                while ((c = getc(fp)) != '\n' && c != EOF)
                    ;   /* skip to end of line */
                return getLine_TOO_LONG;
            }
        }

        return getLine(fp, line, maxlen);
    }
    else
        ungetc(c, fp);


    for (i = 0; i < maxlen && (c = getc(fp)) != '\n' && c != EOF; i++)
        line[i] = (char)c;

    if (i == 0) {
        return c == EOF
               ? getLine_FILE_END
               : getLine(fp, line, maxlen);    /* empty line, read next one */
    }

    if (i == maxlen) {
        if ((c = getc(fp)) == '\n' || c == EOF)
            line[i] = '\0';
        else {
            while ((c = getc(fp)) != '\n' && c != EOF)
                ;   /* skip to end of line */
            return getLine_TOO_LONG;
        }
    }
    else
        line[i] = '\0';

    return i;
}
