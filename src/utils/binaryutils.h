#ifndef BINARYUTILS
#define BINARYUTILS

#include <stdio.h>
#include "operations.h"

#define NUM_BITS    (14)    /* number of bits in a word */
#define MIN_NUMBER  (-8192) /* -(2^(NUM_BITS - 1) */
#define MAX_NUMBER  (8191)  /* (2^(NUM_BITS - 1) - 1) */
#define MIN_IMMEDIATE   (-2048) /* -(2^(NUM_BITS - NUM_ARE_BITS - 1) */
#define MAX_IMMEDIATE   (2047)  /* (2^(NUM_BITS - NUM_ARE_BITS - 1) - 1 */

#define NUM_ARE_BITS    (2) /* number of A,R,E bits */
#define NUM_SOURCE_BITS (2) /* number of bits for the source addressing method */
#define NUM_DEST_BITS   (2) /* number of bits for the destination addressing method */
#define NUM_OPCODE_BITS (4) /* number of bits for the opcode */
#define NUM_REGISTER_BITS   (3) /* number of bits for a register */

#define DEST_REGISTER_START_BIT (2) /* starting encoding bit for destination register operand (when both operands are registers) */
#define SOURCE_REGISTER_START_BIT (5) /* starting encoding bit for source register operand (when both operands are registers) */

#define DEST_OPERAND_ADDRS_START_BIT    (2) /* starting encoding bit for destination operand's addressing method */
#define SRC_OPERAND_ADDRS_START_BIT (4) /* starting encoding bit for source operand's addressing method */

#define BINARY_SYS_BASE (2) /* base of the binary system */

typedef struct {
    int hasValue;
    char bits[NUM_BITS];
} Byte;

int inByteRange(int num);
int numberToByte(int number, Byte *pbyte);
int getFirstWordBin(char opcode, char sourceAddr, char destAddr, Byte *pbyte);
int writeImmediateToByte(Byte *pbyte, int number);
void writeRegisterToByte(Byte *pbyte, int registerNumber, int operandIndex);
void bytesOrGate(Byte b1, Byte b2, Byte *outByte);
void clearByte(Byte *pbyte);
void printByteToFile(Byte byte, FILE *fp);
void getAddrsMethods(int addrMethods[NUM_OPERANDS], Byte operationByte);
void writeAREBits(Byte *byte, int symbolFlags);
void shiftLeft(Byte *byte, int n);
int byteToNumber(Byte byte, int startIndex);

#endif  /* BINARYUTILS */
