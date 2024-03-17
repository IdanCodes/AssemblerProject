#include <math.h>
#include "binaryutils.h"

int abs(int x); /* from <math.h> */
static char getFlippedBit(char b);
static void flipByte(Byte *pbyte);

/* returns whether the conversion was successful
 * - assuming pbyte isn't NULL */
int numberToByte(int number, Byte *pbyte) {
    const int firstBitValue = 1;    /* value of LSB (least significant bit) */
    int bitValue, bitIndex, temp, i;
    int sign;
    
    if (number < MIN_NUMBER || number > MAX_NUMBER)
        return 0;
    
    
    /*
     * 0 0 0 = 0
     * 0 0 1 = 1
     * 0 1 0 = 2
     * 0 1 1 = 3
     * 1 0 0 = -4
     * 1 0 1 = -3
     * 1 1 0 = -2
     * 1 1 1 = -1
     * 
     * -4 -> 4 * sign
     * 4 - 2 = 2 -> [1 0]
     * 2 - 1 = 1 -> [1 1]
     * * sign = [ 1 0 0 ]
     * 
     * 2 -> 2 * sign
     * 2 - 2 = 0 -> [1, 0]
     * * sign = [ 0 1 0 ]
     */
    
    sign = number < 0 ? -1 : 1;
    if (sign < 0)
        number += 1;
    
    temp = (number = abs(number));

    /* find the leftmost used bit */
    for (bitValue = firstBitValue, bitIndex = 0; bitValue <= temp; bitValue *= BINARY_SYS_BASE, bitIndex++) {
        temp -= bitValue;
        pbyte->bits[bitIndex] = 0;
    }

    /* fill the rest with 0s */
    for (i = bitIndex; i < NUM_BITS; i++)
        pbyte->bits[i] = 0;
    
    if (number == 0 && sign > 0)
        return 1;   /* the byte is filled with 0s */
    
    for (; bitValue > 0; bitValue /= BINARY_SYS_BASE, bitIndex--) {
        if (number >= bitValue) {
            number -= bitValue;
            pbyte->bits[bitIndex] = 1;
        }
    }
    
    if (sign < 0)
        flipByte(pbyte);
    
    return 1;
}

/* get a binary representation of an operation's first word
 * - assuming pbyte isn't NULL */
void getFirstWordBin(Operation op, char sourceAddr, char destAddr, Byte *pbyte) {
    int bitIndex;
    const char addressingMethods[][2] = { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 1 } };
    
    /* in the first word of an instruction, the A,R,E bits are 0 */
    for (bitIndex = 0; bitIndex < NUM_ARE_BITS; bitIndex++)
        pbyte->bits[bitIndex] = 0;
    
    /* destination addressing */
    pbyte->bits[bitIndex++] = addressingMethods[destAddr][0];
    pbyte->bits[bitIndex++] = addressingMethods[destAddr][1];
    
    /* source addressing */
    pbyte->bits[bitIndex++] = addressingMethods[sourceAddr][0];
    pbyte->bits[bitIndex++] = addressingMethods[sourceAddr][1];
    
    /* opcode */
    
}

/* flips a bit */
static char getFlippedBit(char b) {
    return b == 0 ? 1 : 0;
}

/* flips all bits in a valid byte */
static void flipByte(Byte *pbyte) {
    int bitIndex;

    for (bitIndex = 0; bitIndex < NUM_BITS; bitIndex++)
        pbyte->bits[bitIndex] = getFlippedBit(pbyte->bits[bitIndex]);
}
