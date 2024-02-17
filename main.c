#include <stdio.h>
#include "src/asm_stages/pre_asm.h"
#include "src/utils/logger.h"
#include "src/utils/fileutils.h"

/* TODO: implement file names from command line input */
#define FILENAME "file.as"

int main(void) {
    FILE *fp;
    
    openFile(FILENAME, "r", &fp);
    preAssemble(fp);

    fclose(fp);

    return 0;
}
