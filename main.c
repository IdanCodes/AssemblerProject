#include <stdio.h>
#include "src/asm_stages/pre_asm.h"
#include "src/asm_stages/first_stage.h"
#include "src/utils/binaryutils.h"

/* TODO: implement file names from command line input */
#define FILENAME "test"

int main(void) {
    /*if (preAssemble(FILENAME) != preAssembleErr_no_err)
        return 1;

    assemblerFirstStage(FILENAME); *//* TODO: check if there was an error after the first stage */
    
    Byte b;
    int i, min = -10, max = 10, number;
    
    for (number = min; number <= max; number++) {
        if (!numberToByte(number, &b)) {
            printf("Could not convert %d.\n", number);
            return 0;
        }

        printf("%d\t=\t[", number);
        for (i = NUM_BITS-1; i >= 0; i--) {
            printf(" %d", b.bits[i]);
        }
        printf(" ]\n");
    }
    
    return 0;
}
