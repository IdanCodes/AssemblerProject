#include <stdlib.h>
#include "symboltype.h"
#include "../utils/charutils.h"
#include "../utils/strutils.h"
#include "../utils/logger.h"

/* DOCUMENT */

/**
 * Is the given string a valid name for a symbol?
 * @param start the start of the symbol name
 * @param end the end of the symbol name (first character *outside* the name)
 * @return 1 if the string is a valid name for a symbol, 0 otherwise
 */
int validSymbolName(char *start, char *end) {
    if (end < start || (end - start) >= LABEL_MAX_LENGTH ||  /* invalid length */
        !isalpha(*start))    /* first character is not alphabetic */
        return 0;

    for (start++; start < end; start++) {
        if (!isalnum(*start))
            return 0;
    }

    return 1;
}

int getSymbolByName(char *name, Symbol *head, Symbol **pSymbol) {
    while (head != NULL) {
        if (tokcmp(head->name, name) == 0) {
            *pSymbol = head;
            return 1;
        }
        head = head->next;
    }

    return 0;
}

int symbolInList(Symbol *head, char *name) {
    Symbol *tempSym;
    return getSymbolByName(name, head, &tempSym);
}

/* nameEnd is the first character outside the name (name string is [name, end-1]) */
Symbol *allocSymbol(char *nameStart, char *nameEnd) {
    Symbol *newS;
    char temp;

    /* TODO: is the name allocated twice? */
    newS = (Symbol *)malloc(sizeof(Symbol));
    if (newS == NULL)
        logInsuffMemErr("allocating symbol");

    temp = *nameEnd;
    *nameEnd = '\0';

    newS->name = strdup(nameStart);
    if (newS->name == NULL)
        logInsuffMemErr("allocating symbol's name");

    *nameEnd = temp;
    return newS;
}

void addSymToList(Symbol **head, Symbol *symbol) {
    Symbol *temp;

    if (*head == NULL) {
        *head = symbol;
        return;
    }

    temp = *head;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = symbol;
}

void freeSymbolsList(Symbol *head) {
    if (head == NULL)
        return;

    freeSymbolsList(head->next);
    free(head->name);
    free(head);
}
