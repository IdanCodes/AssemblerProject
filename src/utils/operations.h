#ifndef OPERATIONS
#define OPERATIONS

#define NUM_OPERANDS    (2)
#define SOURCE_OPERAND_INDEX    (0)
#define DEST_OPERAND_INDEX      (1)

#define ADDR_IMMEDIATE      (0)
#define ADDR_DIRECT         (1)
#define ADDR_CONSTANT_INDEX (2)
#define ADDR_REGISTER       (3)

#define MOV_OPCODE  (0)
#define CMP_OPCODE  (1)
#define ADD_OPCODE  (2)
#define SUB_OPCODE  (3)
#define NOT_OPCODE  (4)
#define CLR_OPCODE  (5)
#define LEA_OPCODE  (6)
#define INC_OPCODE  (7)
#define DEC_OPCODE  (8)
#define JMP_OPCODE  (9)
#define BNE_OPCODE  (10)
#define RED_OPCODE  (11)
#define PRN_OPCODE  (12)
#define JSR_OPCODE  (13)
#define RTS_OPCODE  (14)
#define HLT_OPCODE  (15)

#define NUM_OPERATIONS      (16)
#define NUM_ADDR_METHODS    (4)
#define NUM_MAX_EXTRA_WORDS (4) /* max amount of extra words (after first word) */

typedef struct {
    char opCode;    /* represents a small number, using char to save storage */
    char *opName;
    char sourceAddrMethod[NUM_ADDR_METHODS];    /* represents a small number, using char to save storage */
    char destAddrMethod[NUM_ADDR_METHODS];      /* represents a small number, using char to save storage */
} Operation;

int getOperationByName(char *name, Operation *op);
int getOperandCount(Operation op);
int validAddressingMethod(Operation op, int operandIndex, int addrMethod);
int operationHasOperand(Operation op, int operandIndex);
int getNumWords(Operation op, int sourceAddr, int destAddr);

#endif /* OPERATIONS */
