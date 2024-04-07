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
    secondStageErr_operation_symbol_constant,   /* trying to use a constant as a "non-immediate" operand */
    secondStageErr_operation_index_oor  /* index out of range */
};

int assemblerSecondStage(char fileName[], Symbol *symbols, ByteNode *bytes);

#endif /* SECOND_STAGE */
