#ifndef BINARYUTILS
#define BINARYUTILS

#include "operations.h"

#define NUM_BITS    (14)    /* number of bits in a word */
#define MIN_NUMBER  (-8192) /* -(2^(NUM_BITS - 1) */
#define MAX_NUMBER  (8191)  /* (2^(NUM_BITS - 1) - 1) */

#define NUM_ARE_BITS    (2) /* number of A,R,E bits */

#define BINARY_SYS_BASE (2) /* base of the binary system */

typedef struct {
    char bits[NUM_BITS];
} Byte;

int numberToByte(int number, Byte *pbyte);
void getFirstWordBin(Operation op, char sourceAddr, char destAddr, Byte *pbyte);

#endif  /* BINARYUTILS */
