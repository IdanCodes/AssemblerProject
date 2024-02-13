#include <stdio.h>
#include "utils/inpututils.h"

#define FILENAME "file.as"  /* TODO: implement file names from command line input */

int main(void) {
    int length;
    char line[MAXLINE];
    FILE *fp;

    fp = fopen(FILENAME, "r");

    if (!fp)
        return 1;   /* TODO: print an error instead */

    while ((length = getLine(fp, line, MAXLINE)) != EOF)
        printf("%s (length: %d)\n", line, length);

    fclose(fp);

    return 0;
}
