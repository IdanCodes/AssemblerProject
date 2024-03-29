#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "binaryutils.h"
#include "logger.h"

int abs(int x); /* from <math.h> */
static char getFlippedBit(char b);
static void flipByte(Byte *pbyte);

int inByteRange(int num) {
    return num >= MIN_NUMBER && num <= MAX_NUMBER;
}

/* returns whether the conversion was successful
 * - assuming pbyte isn't NULL */
int numberToByte(int number, Byte *pbyte) {
    const int firstBitValue = 1;    /* byte of LSB (least significant bit) */
    int bitValue, bitIndex, sign, temp, i;
    
    if (!inByteRange(number))
        return 0;
    
    sign = number < 0 ? -1 : 1;
    if (sign < 0)
        number += 1;
    
    temp = (number = number > 0 ? number : -number);

    /* find the leftmost used bit */
    for (bitValue = firstBitValue, bitIndex = 0; bitValue <= temp; bitValue *= BINARY_SYS_BASE, bitIndex++) {
        temp -= bitValue;
        pbyte->bits[bitIndex] = 0;
    }

    /* fill the rest with 0s */
    for (i = bitIndex; i < NUM_BITS; i++)
        pbyte->bits[i] = 0;
    
    pbyte->hasValue = 1;
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
    
    /* unused bits */
    for (; bitIndex < NUM_BITS; bitIndex++)
        pbyte->bits[bitIndex] = 0;
    
    return 1;
}

void clearByte(Byte *pbyte) {
    int i;
    
    for (i = 0; i < NUM_BITS; i++)
        pbyte->bits[i] = 0;
    
    pbyte->hasValue = 1;
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
    
    pbyte->hasValue = 1;
    return 1;
}

void writeRegisterToByte(Byte *pbyte, int registerNumber, int operandIndex) {
    int i, j, startBit;
    Byte tempByte;

    startBit = operandIndex == SOURCE_OPERAND_INDEX ? SOURCE_REGISTER_START_BIT : DEST_REGISTER_START_BIT;
    
    /* clear prefixing bits */
    for (i = 0; i < startBit; i++)
        pbyte->bits[i] = 0;
    
    /* encode into bits */
    numberToByte(registerNumber, &tempByte);
    for (j = 0; j < NUM_REGISTER_BITS; i++, j++)
        pbyte->bits[i] = tempByte.bits[j];

    /* clear the rest of the bits */
    for (; i < NUM_BITS; i++)
        pbyte->bits[i] = 0;
    
    pbyte->hasValue = 1;
}

void bytesOrGate(Byte b1, Byte b2, Byte *outByte) {
    int i;
    
    for (i = 0; i < NUM_BITS; i++)
        outByte->bits[i] = (char)(b1.bits[i] || b2.bits[i]);
}

void printByteToFile(Byte byte, FILE *fp) {
    int i;

    for (i = NUM_BITS - 1; i >= 0; i--)
        fprintf(fp, "%d ", byte.bits[i]);
    fprintf(fp, "\n");
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
