#include <stdio.h>
#include "src/utils/inpututils.h"
#include "src/utils/strutils.h"

#define FILENAME "/Users/idan/Library/CloudStorage/OneDrive-OpenUniversityofIsrael/אוניברסיטה/סמסטרים/2024א/מעבדה בתכנות מערכות/Project/Assembler-1/file.as"  /* TODO: implement file names from command line input */

int main(void) {
    unsigned int lineNumber;
    int length, err;
    char line[MAXLINE];
    FILE *fp;

    fp = fopen(FILENAME, "r");

    if (!fp)
        return 1;   /* TODO: print an error instead */

    lineNumber = 0;
    while ((err = getLine(fp, line, MAXLINE)) != getLine_FILE_END) {
        lineNumber++;
        if (err == getLine_TOO_LONG)
            printf("Line #%d is too long!\n", lineNumber);

        length = trim(line);

        printf("%u. %s (length: %d, previous length/err: %d)\n", lineNumber, line, length, err);
    }

    fclose(fp);

    return 0;
}
