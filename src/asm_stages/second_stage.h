#ifndef SECOND_STAGE
#define SECOND_STAGE

#include "../structures/symboltype.h"
#include "../structures/bytelist.h"

enum secondStageErr {
    secondStageErr_no_err,  /* no error */
    
    /* .entry */
    secondStageErr_entry_undefined, /* .entry parameter's symbol was not defined */
    secondStageErr_entry_symbol_is_extern,  /* trying to define an extern symbol as entry */
    secondStageErr_entry_constant,  /* trying to define a constant as entry */
    
    /* operation */
    secondStageErr_operation_symbol_undefined,  /* undefined symbol as operand */
    secondStageErr_operation_symbol_constant    /* trying to use a constant as a "non-immediate" operand */
};

int assemblerSecondStage(char fileName[], int *data, Symbol *symbols, ByteNode *bytes, int numInstructions, int dataCounter);

#endif /* SECOND_STAGE */
