#include <stdio.h>
#include <stdlib.h>
#include "src/asm_stages/pre_asm.h"
#include "src/asm_stages/first_stage.h"
#include "src/asm_stages/second_stage.h"
#include "src/utils/binaryutils.h"
#include "src/utils/fileutils.h"
#include "src/utils/logger.h"
#include "src/structures/symboltype.h"

/* TODO: implement file names from command line input */
#define FILENAME "test"

int main(void) {
    int *data;
    Symbol *symbols;
    ByteNode *bytes;
    int hasErr, i, instructionCounter, dataCounter;
    char objectFileName[FILENAME_MAX], entFileName[FILENAME_MAX], extFileName[FILENAME_MAX];
    FILE *objf;
    
    sprintf(objectFileName, "%s.%s", FILENAME, OBJECT_FILE_EXTENSION);
    sprintf(entFileName, "%s.%s", FILENAME, ENTRIES_FILE_EXTENSION);
    sprintf(extFileName, "%s.%s", FILENAME, EXTERNALS_FILE_EXTENSION);
    
    hasErr = !preAssemble(FILENAME);
    
    if (hasErr) {
        logInfo("Encounterd an error in pre-assembler.\n");
        
        /* try to delete output files (if they exist) */
        tryDeleteFile(objectFileName);
        tryDeleteFile(entFileName);
        tryDeleteFile(extFileName);
        
        return 1;
    }
    
    hasErr = assemblerFirstStage(FILENAME, &data, &symbols, &bytes, &instructionCounter, &dataCounter);
    hasErr = hasErr || (assemblerSecondStage(FILENAME, symbols, bytes));
    
    openFile(objectFileName, "w", &objf);
    
    if (hasErr) {
        logInfo("The assembler encountered an error.\n");
        
        /* delete the output files */
        fclose(objf);
        deleteFile(objectFileName);
        tryDeleteFile(entFileName);
        tryDeleteFile(extFileName);
        
        /* free allocated memory */
        free(data);
        freeSymbolsList(symbols);
        freeByteList(bytes);
        return 1;
    }
    else {
        /* write to object file */
        fprintf(objf, "%d %d\n", instructionCounter, dataCounter);
        for (i = INSTRUCTION_COUNTER_OFFSET; bytes != NULL; bytes = bytes->next, i++) {
            fprintf(objf, "%04d ", i);  /* print instruction number in memory */
            printBase4(objf, bytes->byte);
            printByteToFile(bytes->byte, stdout);
            putc('\n', objf);
        }
    }
    
    /* close open file */
    
    
    /* free allocated data */
    free(data);
    freeSymbolsList(symbols);
    freeByteList(bytes);
    
    return hasErr;
}
