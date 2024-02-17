#include <stdio.h>
#include "src/utils/inpututils.h"
#include "src/utils/strutils.h"
#include "src/asm_stages/pre_asm.h"

#define FILENAME "/Users/idan/Library/CloudStorage/OneDrive-OpenUniversityofIsrael/אוניברסיטה/סמסטרים/2024א/מעבדה בתכנות מערכות/Project/Assembler-1/file.as"  /* TODO: implement file names from command line input */

int main(void) {
    FILE *fp;

    fp = fopen(FILENAME, "r");

    if (!fp)
        return 1;   /* TODO: print an error instead */

    preAssemble(fp);

    fclose(fp);

    return 0;
}
