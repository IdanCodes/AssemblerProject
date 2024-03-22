#include "operations.h"
#include "strutils.h"
#include "keywords.h"



/* DOCUMENT functions in this file */
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

int getOperandCount(Operation op) {
    return operationHasOperand(op, SOURCE_OPERAND_INDEX) + operationHasOperand(op, DEST_OPERAND_INDEX);
}

/**
 * Check if an operation accepts an addressing method for an operand.
 * @param op the operation to check
 * @param operandIndex the index of the operand to check
 * @param addressingMethod the addressing method to check
 * @return whether the operation accepts the addressing method
 */
int validAddressingMethod(Operation op, int operandIndex, int addrMethod) {
    if (addrMethod < 0 || addrMethod >= NUM_ADDR_METHODS)
        return 0;
        
    return operandIndex == SOURCE_OPERAND_INDEX
            ? op.sourceAddrMethod[addrMethod]
            : operandIndex == DEST_OPERAND_INDEX
                ? op.destAddrMethod[addrMethod]
                : 0;
}

/**
 * Does the operation accept an operand at the given operand index?
 * @param op the given operation
 * @param operandIndex the operand index to check
 * @return whether the operation accepts the given operand index
 */
int operationHasOperand(Operation op, int operandIndex) {
    return operandIndex == SOURCE_OPERAND_INDEX
            ? (op.sourceAddrMethod[ADDR_IMMEDIATE] || op.sourceAddrMethod[ADDR_DIRECT]
               || op.sourceAddrMethod[ADDR_CONSTANT_INDEX] || op.sourceAddrMethod[ADDR_REGISTER])
            : operandIndex == DEST_OPERAND_INDEX
                ? (op.destAddrMethod[ADDR_IMMEDIATE] || op.destAddrMethod[ADDR_DIRECT]
                   || op.destAddrMethod[ADDR_CONSTANT_INDEX] || op.sourceAddrMethod[ADDR_REGISTER])
                : 0;    /* invalid operand index */
}
