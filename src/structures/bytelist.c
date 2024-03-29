#include <stdlib.h>
#include "bytelist.h"
#include "../utils/logger.h"

ByteNode *allocByteNode(void) {
    ByteNode *value;

    value = (ByteNode *)malloc(sizeof(ByteNode));
    if (value == NULL)
        logInsuffMemErr("allocating ByteNode");

    value->next = NULL;

    return value;
}

void addByteNodeToList(ByteNode **head, ByteNode *node) {
    ByteNode *temp;
    
    if (*head == NULL) {
        *head = node;
        return;
    }
    
    temp = *head;

    while (temp->next != NULL)
        temp = temp->next;

    temp->next = node;
}

/* allocates a ByteNode and copies the values from the give byte */
ByteNode *copyByte(Byte src) {
    int i;
    ByteNode *result;
    
    result = allocByteNode();
    
    result->byte.hasValue = src.hasValue;
    if (src.hasValue) {
        for (i = 0; i < NUM_BITS; i++)
            result->byte.bits[i] = src.bits[i];
    }
    
    return result;
}

void freeByteList(ByteNode *head) {
    if (head == NULL)
        return;

    freeByteList(head->next);
    free(head);
}
