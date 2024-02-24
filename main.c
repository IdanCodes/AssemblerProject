#include <stdio.h>
#include "src/asm_stages/pre_asm.h"

/* TODO: implement file names from command line input */
#define FILENAME "file"

int main(void) {
    preAssemble(FILENAME);
    return 0;
}
