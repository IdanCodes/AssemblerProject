#ifndef SECOND_STAGE
#define SECOND_STAGE

#include "../structures/symboltype.h"
#include "../structures/bytelist.h"

enum secondStageErr {
    secondStageErr_no_err,  /* no error */
    
    /* .entry */
    secondStageErr_entry_undefined  /* .entry parameter's symbol was not defined */
};

int assemblerSecondStage(char fileName[], int *data, Symbol *symbols, ByteNode *bytes, int numInstructions, int dataCounter);

#endif /* SECOND_STAGE */
