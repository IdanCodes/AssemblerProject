#include <stdlib.h>
#include "bytelist.h"
#include "../utils/logger.h"

/**
 * Allocate a ByteNode
 * @return A pointer to the allocated ByteNode
 */
ByteNode *allocByteNode(void) {
    ByteNode *value;

    value = (ByteNode *)malloc(sizeof(ByteNode));
    if (value == NULL)
        logInsuffMemErr("allocating ByteNode");

    value->next = NULL;

    return value;
}

/**
 * Add a ByteNode to a ByteNode list
 * @param head A pointer to the head of the ByteNode list
 * @param node The ByteNode to add to the list
 */
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

/**
 * Allocates a ByteNode and copies the values from a byte
 * @param src The Byte to copy
 * @return The ByteNode allocated
 */
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

/**
 * Free a ByteNode list
 * @param head The head of the ByteNode list
 */
void freeByteList(ByteNode *head) {
    if (head == NULL)
        return;

    freeByteList(head->next);
    free(head);
}
