#include <stdio.h>
#include <stdlib.h>
#include "src/asm_stages/pre_asm.h"
#include "src/asm_stages/first_stage.h"
#include "src/utils/binaryutils.h"
#include "src/utils/fileutils.h"
#include "src/utils/logger.h"

/* TODO: implement file names from command line input */
#define FILENAME "test"

int main(void) {
    char binFileName[FILENAME_MAX]/*, objFileName[FILENAME_MAX]*/;
    int *data;
    Symbol *symbols;
    int hasErr;
    
    hasErr = !preAssemble(FILENAME);
    
    if (hasErr) {
        logInfo("Encounterd an error in pre-assembler.\n");
        return 1;
    }

    sprintf(binFileName, "%s.%s", FILENAME, BINARY_FILE_EXTENSION);
    
    hasErr = assemblerFirstStage(FILENAME, &data, &symbols);
    /*hasErr = hasErr || (assemblerSecondStage(FILENAME, &data, &symbols));*/

    if (hasErr)
        logInfo("The assembler encountered an error.\n");
    
    free(data);
    freeSymbolsList(symbols);
    /*deleteFile(binFileName);*/
    
    return 0;
}
