#include "operations.h"
#include "strutils.h"
#include "keywords.h"

static Operation operations[NUM_OPERATIONS] = {
    {
        MOV_OPCODE,
        KEYWORD_OPERATION_MOV,
        { 1, 1, 1, 1 },
        { 0, 1, 1, 1 },
    },

    {
        CMP_OPCODE,
        KEYWORD_OPERATION_CMP,
        { 1, 1, 1, 1 },
        { 1, 1, 1, 1 },
    },

    {
        ADD_OPCODE,
        KEYWORD_OPERATION_ADD,
        { 1, 1, 1, 1 },
        { 0, 1, 1, 1 }
    },

    {
        SUB_OPCODE,
        KEYWORD_OPERATION_SUB,
        { 1, 1, 1, 1 },
        { 0, 1, 1, 1 }
    },
    
    {
        NOT_OPCODE,
        KEYWORD_OPERATION_NOT,
        { 0, 0, 0, 0 },
        { 0, 1, 1, 1 }
    },
    
    {
        CLR_OPCODE,
        KEYWORD_OPERATION_CLR,
        { 0, 0, 0, 0 },
        { 0, 1, 1, 1 }
    },
    
    {
        LEA_OPCODE,
        KEYWORD_OPERATION_LEA,
        { 0, 1, 1, 0 },
        { 0, 1, 1, 1 }
    },
    
    {
        INC_OPCODE,
        KEYWORD_OPERATION_INC,
        { 0, 0, 0, 0 },
        { 0, 1, 1, 1 }
    },
    
    {
        DEC_OPCODE,
        KEYWORD_OPERATION_DEC,
        { 0, 0, 0, 0 },
        { 0, 1, 1, 1 }
    },

    {
        JMP_OPCODE,
        KEYWORD_OPERATION_JMP,
        { 0, 0, 0, 0 },
        { 0, 1, 0, 1 }
    },

    {
        BNE_OPCODE,
        KEYWORD_OPERATION_BNE,
        { 0, 0, 0, 0 },
        { 0, 1, 0, 1 }
    },

    {
        RED_OPCODE,
        KEYWORD_OPERATION_RED,
        { 0, 0, 0, 0 },
        { 0, 1, 1, 1 }
    },

    {
        PRN_OPCODE,
        KEYWORD_OPERATION_PRN,
        { 0, 0, 0, 0 },
        { 1, 1, 1, 1 }
    },

    {
        JSR_OPCODE,
        KEYWORD_OPERATION_JSR,
        { 0, 0, 0, 0 },
        { 0, 1, 0, 1 }
    },

    {
        RTS_OPCODE,
        KEYWORD_OPERATION_RTS,
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 }
    },

    {
        HLT_OPCODE,
        KEYWORD_OPERATION_HLT,
        { 0, 0, 0, 0 },
        { 0, 0, 0, 0 }
    }
};

int getOperationByName(char *name, Operation *op) {
    int i;
    
    for (i = 0; i < NUM_OPERATIONS; i++) {
        if (tokcmp(name, operations[i].opName) == 0) {
            *op = operations[i];
            return 1;
        }
    }
    
    return 0;
}
