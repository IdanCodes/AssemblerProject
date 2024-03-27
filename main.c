#include <stdio.h>
#include "src/asm_stages/pre_asm.h"
#include "src/asm_stages/first_stage.h"
#include "src/utils/binaryutils.h"

/* TODO: implement file names from command line input */
#define FILENAME "test"

int main(void) {
    int *data;
    Symbol *symbols;
    
    if (preAssemble(FILENAME) != preAssembleErr_no_err)
        return 1;

    if (!assemblerFirstStage(FILENAME, &data, &symbols))
        return 1;
    
    return 0;
}
