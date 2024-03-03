#include <stdio.h>
#include "src/asm_stages/pre_asm.h"
#include "src/asm_stages/first_stage.h"

/* TODO: implement file names from command line input */
#define FILENAME "test"

int main(void) {
    if (preAssemble(FILENAME) != preAssembleErr_no_err)
        return 1;

    assemblerFirstStage(FILENAME); /* TODO: check if there was an error after the first stage */
    
    return 0;
}
