#include <stdio.h>
#include "src/asm_stages/pre_asm.h"
#include "src/asm_stages/first_stage.h"
#include "src/utils/binaryutils.h"

/* TODO: implement file names from command line input */
#define FILENAME "test"

int main(void) {
    /*if (preAssemble(FILENAME) != preAssembleErr_no_err)
        return 1;

    assemblerFirstStage(FILENAME);  *//* TODO: check if there was an error after the first stage */
    
    int i;
    char opcode = 9, srcAddr = 3, destAddr = 2;
    Byte b;
    
    if (!getFirstWordBin(opcode, srcAddr, destAddr, &b)) {
        printf("Couldn't convert to binary.\n");
    }
    
    for (i = NUM_BITS-1; i >= 0; i--) {
        printf(" %d", b.bits[i]);
    }
    
    return 0;
}
