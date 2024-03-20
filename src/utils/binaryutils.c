#include <stdio.h>
#include <math.h>
#include "binaryutils.h"

int abs(int x); /* from <math.h> */
static char getFlippedBit(char b);
static void flipByte(Byte *pbyte);

/* returns whether the conversion was successful
 * - assuming pbyte isn't NULL */
int numberToByte(int number, Byte *pbyte) {
    const int firstBitValue = 1;    /* value of LSB (least significant bit) */
    int bitValue, bitIndex, sign, temp, i;
    
    if (number < MIN_NUMBER || number > MAX_NUMBER)
        return 0;
    
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
 * - assuming pbyte isn't NULL
 * - returns whether the conversion was successful */
int getFirstWordBin(char opcode, char sourceAddr, char destAddr, Byte *pbyte) {
    int start, bitIndex;
    Byte tempByte;
    
    /* in the first word of an instruction, the A,R,E bits are 0 */
    for (bitIndex = 0; bitIndex < NUM_ARE_BITS; bitIndex++)
        pbyte->bits[bitIndex] = 0;
    
    /* destination addressing */
    if (!numberToByte(destAddr, &tempByte))
        return 0;
    for (start = bitIndex; (bitIndex - start) < NUM_DEST_BITS; bitIndex++)
        pbyte->bits[bitIndex] = tempByte.bits[bitIndex - start];
    
    
    /* source addressing */
    if (!numberToByte(sourceAddr, &tempByte))
        return 0;
    for (start = bitIndex; (bitIndex - start) < NUM_SOURCE_BITS; bitIndex++)
        pbyte->bits[bitIndex] = tempByte.bits[bitIndex - start];
    
    /* opcode */
    if (!numberToByte(opcode, &tempByte))
        return 0;
    for (start = bitIndex; (bitIndex - start) < NUM_OPCODE_BITS; bitIndex++)
        pbyte->bits[bitIndex] = tempByte.bits[bitIndex - start];
    
    for (; bitIndex < NUM_BITS; bitIndex++)
        pbyte->bits[bitIndex] = 0;
    
    return 1;
}

void clearByte(Byte *pbyte) {
    int i;
    
    for (i = 0; i < NUM_BITS; i++)
        pbyte->bits[i] = 0;
}

/* returns whether the writing was successful (pretty much whether the number was in range) */
int writeImmediateToByte(Byte *pbyte, int number) {
    int i;
    int j;
    Byte tempByte;
    
    if (number < MIN_IMMEDIATE || number > MAX_IMMEDIATE)
        return 0;
    
    clearByte(pbyte);
    
    /* A,R,E bits are 0 */
    i = NUM_ARE_BITS;
    numberToByte(number, &tempByte);
    
    /* copy the bits */
    for (j = 0; i < NUM_BITS; i++, j++)
        pbyte->bits[i] = tempByte.bits[j];
    
    return 1;
}

/* TODO: THIS IS FOR TESTING ONLY (maybe make this print to a file in fileutils.c or something) */
void printByte(Byte byte) {
    int i;
    
    for (i = NUM_BITS - 1; i >= 0; i--)
        printf(" %d", byte.bits[i]);
    printf("\n");
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
