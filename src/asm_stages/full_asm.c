#include <stdlib.h>
#include "full_asm.h"
#include "../structures/symboltype.h"
#include "../structures/bytelist.h"
#include "../utils/fileutils.h"
#include "pre_asm.h"
#include "../utils/logger.h"
#include "first_stage.h"
#include "second_stage.h"

int assembleFile(char FILENAME[FILENAME_MAX]) {
    int *data;
    Symbol *symbols;
    ByteNode *bytes;
    int hasErr, i, instructionCounter, dataCounter;
    char sourceFileName[FILENAME_MAX], objectFileName[FILENAME_MAX], entFileName[FILENAME_MAX], extFileName[FILENAME_MAX];
    FILE *objf;

    sprintf(sourceFileName, "%s.%s", FILENAME, SOURCE_FILE_EXTENSION);
    sprintf(objectFileName, "%s.%s", FILENAME, OBJECT_FILE_EXTENSION);
    sprintf(entFileName, "%s.%s", FILENAME, ENTRIES_FILE_EXTENSION);
    sprintf(extFileName, "%s.%s", FILENAME, EXTERNALS_FILE_EXTENSION);
    
    logInfo("Assembling file \"%s\"...\n", sourceFileName);
    hasErr = preAssemble(FILENAME);

    if (hasErr) {
        /* try to delete output files (if they exist) */
        tryDeleteFile(objectFileName);
        tryDeleteFile(entFileName);
        tryDeleteFile(extFileName);

        goto end;
    }

    hasErr = assemblerFirstStage(FILENAME, &data, &symbols, &bytes, &instructionCounter, &dataCounter);
    hasErr = hasErr || (assemblerSecondStage(FILENAME, symbols, bytes));

    if (hasErr) {
        logInfo("The assembler encountered an error.\n");

        /* delete the output files */
        tryDeleteFile(objectFileName);
        tryDeleteFile(entFileName);
        tryDeleteFile(extFileName);
    }
    else {
        /* write to object file */
        openFile(objectFileName, "w", &objf);
        
        fprintf(objf, "%d %d\n", instructionCounter, dataCounter);
        for (i = INSTRUCTION_COUNTER_OFFSET; bytes != NULL; bytes = bytes->next, i++) {
            fprintf(objf, "%04d ", i);  /* print instruction number in memory */
            printBase4(objf, bytes->byte);
            putc('\n', objf);
        }

        fclose(objf);
        logInfo("Finished assembling file \"%s\" with no errors.\n", sourceFileName);
    }

    /* free allocated data */
    free(data);
    freeSymbolsList(symbols);
    freeByteList(bytes);
    
    end:
    printf("\n");   /* space out logs */
    return hasErr;
}
