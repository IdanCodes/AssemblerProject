#include <stdio.h>
#include <math.h>
#include "binaryutils.h"
#include "../structures/symboltype.h"
#include "logger.h"

static char getFlippedBit(char b);
static void flipByte(Byte *pbyte);

/**
 * Check if a number is in the range of a byte
 * @param num The number to check
 * @return Whether the number is in range
 */
int inByteRange(int num) {
    const int MAX_BYTE = 1 << (NUM_BITS - 1);
    const int MIN_BYTE = -MAX_BYTE - 1;
    
    return num > MIN_BYTE && num < MAX_BYTE;
}

/**
 * Convert a number to a byte
 * @param number The number to convert
 * @param pbyte A pointer to store the conversion into
 * @return whether the conversion to byte was successful (fails if the number is out of range for a byte or pbyte==NULL)
 */
int numberToByte(int number, Byte *pbyte) {
    const int firstBitValue = 1;    /* byte of LSB (least significant bit) */
    int bitValue, bitIndex, sign, temp, i;
    
    if (pbyte == NULL || !inByteRange(number))
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

/**
 * Get a binary representation of an operation's first word
 * @param opcode The operation
 * @param sourceAddr The source addressing method
 * @param destAddr The destination addressing method
 * @param pbyte A pointer to the byte to store the word into
 * @return Whether the conversion was successful
 */
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
    
    return pbyte->hasValue = 1;
}

/**
 * Clear a byte's bits
 * @param pbyte A pointer to the byte to clear
 */
void clearByte(Byte *pbyte) {
    int i;
    
    /* can't clear a null pointer */
    if (pbyte == NULL)
        return;
    
    for (i = 0; i < NUM_BITS; i++)
        pbyte->bits[i] = 0;
    
    pbyte->hasValue = 1;
}

/**
 * Write an immediate number to a byte
 * @param pbyte A pointer to the byte to write to
 * @param number The number to write to the byte
 * @return Whether the writing was successful (pretty much whether the number was in range)
 */
int writeImmediateToByte(Byte *pbyte, int number) {
    int i;
    int j;
    Byte tempByte;
    
    /* number << 2 is the number after adding the A,R,E bits */
    if (!inByteRange(number << 2))
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

/**
 * Write a register operand to a byte
 * @param pbyte A pointer to the byte to write to
 * @param registerNumber The number of the register to write
 * @param operandIndex The index of the operand (source, dest)
 */
void writeRegisterToByte(Byte *pbyte, int registerNumber, int operandIndex) {
    int i, j, startBit;
    Byte tempByte;

    if (pbyte == NULL)
        return; /* can't write to NULL */
    
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

/**
 * Logical or gate on two bytes (individual or operation on each bit)
 * @param b1 The first byte
 * @param b2 The second byte
 * @param outByte The result of the logical or gate of the bytes
 */
void bytesOrGate(Byte b1, Byte b2, Byte *outByte) {
    int i;
    
    for (i = 0; i < NUM_BITS; i++)
        outByte->bits[i] = (char)(b1.bits[i] || b2.bits[i]);
}

/**
 * Shift the given byte n bits to the left
 * @param byte the byte to shift
 * @param n the number of bits to shift
 */
void shiftLeft(Byte *byte, int n) {
    int i;
    
    if (n == 0)
        return;
    
    if (n >= NUM_BITS) {
        clearByte(byte);
        return;
    }
    
    if (n < 0) {
        logWarn("shiftLeft function called with negative shift amount - using absolute value\n");
        n = -n;
    }
    
    for (i = NUM_BITS - 1; i >= n; i--)
        byte->bits[i] = byte->bits[i-n];
    
    /* reset new bits */
    for (; i >= 0; i--)
        byte->bits[i] = 0;
}

/**
 * Get the addressing methods described in a first word byte
 * @param addrMethods An array of address methods to store the address methods into
 * @param operationByte The byte to read from
 */
void getAddrsMethods(int addrMethods[NUM_OPERANDS], Byte operationByte) {
    int i;
    
    addrMethods[SOURCE_OPERAND_INDEX] = 0;
    addrMethods[DEST_OPERAND_INDEX] = 0;
    
    for (i = 0; i < NUM_SOURCE_BITS; i++) {
        if (operationByte.bits[i + SRC_OPERAND_ADDRS_START_BIT] == 1)
            addrMethods[SOURCE_OPERAND_INDEX] += (int)pow(2, i);
        
        if (operationByte.bits[i + DEST_OPERAND_ADDRS_START_BIT] == 1)
            addrMethods[DEST_OPERAND_INDEX] += (int)pow(2, i);
    }
}

/**
 * Write A,R,E bits to a byte
 * @param byte The byte to write to
 * @param symbolFlags The symbol flags (for the A,R,E bits
 */
void writeAREBits(Byte *byte, int symbolFlags) {
    const char ext[NUM_ARE_BITS] = { 1, 0 };
    const char relocatable[NUM_ARE_BITS] = { 0, 1 };
    
    int i;
    char *bits;
    
    bits = (char *)(((symbolFlags & SYMBOL_FLAG_EXTERN) != 0) ? ext : relocatable);
    
    for (i = 0; i < NUM_ARE_BITS; i++)
        byte->bits[i] = bits[i];
}

/**
 * Converts a byte into a number
 * @param byte the given byte
 * @param startIndex the bit startIndex to start reading from
 * @return the number the byte represents
 */
int byteToNumber(Byte byte, int startIndex) {
    int i, result;

    startIndex = startIndex >= 0 ? startIndex : 0;
    
    for (i = startIndex, result = 0; i < NUM_BITS; i++) {
        if (byte.bits[i] != 0)
            result += (int)pow(BINARY_SYS_BASE, i - startIndex);
    }
    
    return result;
}

/**
 * Print a byte in base 4 to a file
 * @param fp A FILE pointer to the file to write to 
 * @param byte The byte to write to the file
 */
void printBase4(FILE *fp, Byte byte) {
    int i;
    
    /* reading the byte in reverse and saving inside base4 */
    for (i = NUM_BITS_BASE_4 - 1; i >= 0; i--) {
        putc(byte.bits[2 * i + 1] == 0  /* second bit */
            ? ((byte.bits[2 * i] == 0)  /* first bit */
                /* second bit off */
                ? BASE_4_ZERO   /* first bit off */
                : BASE_4_ONE)   /* first bit on */
            : ((byte.bits[2 * i] == 0)  /* first bit */
                /* second bit on */
                ? BASE_4_TWO    /* first bit off */
                : BASE_4_THREE) /* first bit on */
            , fp);
        
    }
}

/**
 * Flips a bit (0<->1)
 * @param b The bit
 * @return The flipped bit
 */
static char getFlippedBit(char b) {
    return b == 0 ? 1 : 0;
}

/* flips all bits in a valid byte */
/**
 * Flips all bits in a byte
 * @param pbyte A pointer to the byte to flip
 */
static void flipByte(Byte *pbyte) {
    int bitIndex;

    for (bitIndex = 0; bitIndex < NUM_BITS; bitIndex++)
        pbyte->bits[bitIndex] = getFlippedBit(pbyte->bits[bitIndex]);
}
