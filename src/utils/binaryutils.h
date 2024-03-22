#ifndef BINARYUTILS
#define BINARYUTILS

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


#define BINARY_SYS_BASE (2) /* base of the binary system */

typedef struct {
    int hasValue;
    char bits[NUM_BITS];
} Byte;

int numberToByte(int number, Byte *pbyte);
int getFirstWordBin(char opcode, char sourceAddr, char destAddr, Byte *pbyte);
int writeImmediateToByte(Byte *pbyte, int number);
void clearByte(Byte *pbyte);
void printByte(Byte byte);

#endif  /* BINARYUTILS */
