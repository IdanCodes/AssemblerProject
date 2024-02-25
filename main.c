#include <stdio.h>
#include "src/asm_stages/pre_asm.h"

/* TODO: implement file names from command line input */
#define FILENAME "file"

int main(void) {
    if (preAssemble(FILENAME) != preAssembleErr_no_err)
        return 1;
    
    
    
    return 0;
}
