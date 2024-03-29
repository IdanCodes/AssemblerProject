#ifndef BYTELIST
#define BYTELIST

#include "../utils/binaryutils.h"

typedef struct ByteNode {
    Byte byte;
    struct ByteNode *next;
} ByteNode;

ByteNode *allocByteNode(void);
ByteNode *copyByte(Byte src);
void addByteNodeToList(ByteNode **head, ByteNode *node);
void freeByteList(ByteNode *head);

#endif /* BYTELIST */
