#include "operations.h"
#include "../utils/strutils.h"

static Operation operations[NUM_OPERATIONS] = {
    {
        MOV_OPCODE,
        "mov",
        { 1, 1, 1, 1 },
        {0, 1, 1, 1},
    },

    {
        CMP_OPCODE,
        "cmp",
        { 1, 1, 1, 1 },
        { 1, 1, 1, 1 },
    },

    {
        ADD_OPCODE,
        "add",
        { 1, 1, 1, 1 },
        { 0, 1, 1, 1 }
    },

    {
        SUB_OPCODE,
        "sub",
        { 1, 1, 1, 1 },
        { 0, 1, 1, 1 }
    },
    
    {
        NOT_OPCODE,
        "not",
        { },
        { 0, 1, 1, 1 }
    },
    
    {
        CLR_OPCODE,
        "clr",
        { },
        { 0, 1, 1, 1 }
    },
    
    {
        LEA_OPCODE,
        "lea",
        { 0, 1, 1, 0 },
        { 0, 1, 1, 1 }
    },
    
    {
        INC_OPCODE,
        "inc",
        { },
        { 0, 1, 1, 1 }
    },
    
    {
        DEC_OPCODE,
        "dec",
        { },
        { 0, 1, 1, 1 }
    },

    {
        JMP_OPCODE,
        "jmp",
        { },
        { 0, 1, 0, 1 }
    },

    {
        BNE_OPCODE,
        "bne",
        { },
        { 0, 1, 0, 1 }
    },

    {
        RED_OPCODE,
        "red",
        { },
        { 0, 1, 1, 1 }
    },

    {
        PRN_OPCODE,
        "prn",
        { },
        { 1, 1, 1, 1 }
    },

    {
        JSR_OPCODE,
        "jsr",
        { },
        { 0, 1, 0, 1 }
    },

    {
        RTS_OPCODE,
        "rts",
        { },
        { }
    },

    {
        HLT_OPCODE,
        "hlt",
        { },
        { }
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
