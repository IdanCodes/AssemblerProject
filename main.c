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
    ByteNode *bytes, *temp;
    int hasErr, i, instructionCounter, dataCounter;
    
    hasErr = !preAssemble(FILENAME);
    
    if (hasErr) {
        logInfo("Encounterd an error in pre-assembler.\n");
        return 1;
    }
    
    /* TODO: length check for arrays access */
    
    hasErr = assemblerFirstStage(FILENAME, &data, &symbols, &bytes, &instructionCounter, &dataCounter);
    
    temp = bytes;
    logInfo("Instructions:\n");
    for (i = 0; i < instructionCounter; i++) {
        printf("%d ", i + INSTRUCTION_COUNTER_OFFSET);
        if (bytes->byte.hasValue)
            printByteToFile(bytes->byte, stdout);
        else
            printf("?\n");
        bytes = bytes->next;
    }

    logInfo("Data:\n");
    for (i = 0; i < dataCounter; i++) {
        printf("%d ", instructionCounter + i + INSTRUCTION_COUNTER_OFFSET);
        printByteToFile(bytes->byte, stdout);
        bytes = bytes->next;
    }
    
    printf("\n\n\n\n\n\n\n\n\n\n");
    bytes = temp;
    
    hasErr = hasErr || (assemblerSecondStage(FILENAME, data, symbols, bytes, instructionCounter, dataCounter));
    
    if (hasErr)
        logInfo("The assembler encountered an error.\n");
    else {
        logInfo("Instructions:\n");
        for (i = 0; i < instructionCounter; i++) {
            printf("%d ", i + INSTRUCTION_COUNTER_OFFSET);
            if (bytes->byte.hasValue)
                printByteToFile(bytes->byte, stdout);
            else
                printf("?\n");
            bytes = bytes->next;
        }
        
        logInfo("Data:\n");
        for (i = 0; i < dataCounter; i++) {
            printf("%d ", instructionCounter + i + INSTRUCTION_COUNTER_OFFSET);
            printByteToFile(bytes->byte, stdout);
            bytes = bytes->next;
        }
    }
    
    free(data);
    freeSymbolsList(symbols);
    freeByteList(bytes);
    
    return 0;
}
