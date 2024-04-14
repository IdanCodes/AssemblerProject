#include "src/asm_stages/full_asm.h"
#include "src/utils/logger.h"
#include "src/utils/strutils.h"

int main(int argc, char *argv[]) {
    int i, j;
    char *arg;
    
    if (argc == 1) {
        logWarn("At least one file required to assemble.\n");
        return 0;
    }

    i = 1;
    arg = argv[i]; /* first argument is the program's name */
    for (; i < argc; i++, arg = argv[i]) {
        /* check if this file has already been assembled */
        for (j = 1; j < i; j++) {
            if (tokcmp(arg, argv[j]) == 0)
                break;
        }
        
        if (j < i) /* if the loop was stopped in the middle */
            continue;   /* don't assemble the same file twice */
        
        assembleFile(arg);
    }
    
    return 0;
}
