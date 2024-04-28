#include <stdlib.h>
#include <string.h>
#include "macrotype.h"
#include "../utils/logger.h"
#include "../utils/strutils.h"
#include "../utils/inpututils.h"
#include "../utils/fileutils.h"
#include "../utils/charutils.h"
#include "symboltype.h"

/**
 * Allocate a Macro
 * @param name the name of the macro
 * @return the address of the allocated macro
 */
Macro *allocMcr(char *name) {
    char temp, *nameEnd;
    Macro *result;

    result = (Macro *)malloc(sizeof(Macro));
    if (result == NULL)
        logInsuffMemErr("allocating macro");

    nameEnd = getTokEnd(name) + 1;
    temp = *nameEnd;
    *nameEnd = '\0';
    
    result->name = malloc(strlen(name));
    strcpy(result->name, name);
    
    if (result->name == NULL)
        logInsuffMemErr("allocating macro's name");

    result->next = NULL;
    *nameEnd = temp;
    
    return result;
}

/**
 * Check if a macro with the given name exists (in the given list)
 * @param name the name to search
 * @param head the head of the list to search in
 * @return if the macro was found, returns its address. otherwise returns NULL.
 */
Macro *getMacroWithName(char *name, Macro *head) {
    while (head != NULL) {
        if (tokcmp(name, head->name) == 0)
            return head;
        head = head->next;
    }
    return NULL;
}

/**
 * Expand a macro into a file
 * @param mcr the macro to expand
 * @param sourcef the file to expand from
 * @param destf the file to expand to
 */
void expandMacro(Macro *mcr, FILE *destf, char *sourcefileName) {
    unsigned int sourceLine, skippedLines;
    char line[MAXLINE + 1];
    FILE *sourcef;

    openFile(sourcefileName, "r", &sourcef);

    for (sourceLine = 0, skippedLines = 0; sourceLine < mcr->startLine-1; sourceLine += skippedLines) {
        if (getNextLine(sourcef, line, 1, MAXLINE, &skippedLines) == getLine_FILE_END)
            /* this will never happen */
            terminalError(1, "Error expanding macro '%s' - reached end of file (in file '%s').\n", mcr->name, sourcefileName);
    }

    getNextLine(sourcef, line, 1, MAXLINE, &skippedLines);
    sourceLine += skippedLines;
    for (; sourceLine < mcr->endLine;) {
        fprintf(destf, "%s\n", line);
        getNextLine(sourcef, line, 1, MAXLINE, &skippedLines);
        sourceLine += skippedLines;
    }

    fclose(sourcef);
}

/**
 * check if a macro's name is valid
 * @param name the name to check
 * @return whether the given name was valid
 */
int validMcrName(char *name) {
    int i;
    
    if (!isalpha(*name))
        return 0;
    
    for (i = 1; i < LABEL_MAX_LENGTH && !isspace(name[i]) && name[i] != '\0'; i++) {
        /* only allow alphanumeric characters and underscores */
        if (!isalnum(name[i]) && name[i] != '_')
            return 0;
    }
    
    return i <= LABEL_MAX_LENGTH && (isspace(name[i]) || name[i] == '\0');
}

/**
 * Free (deallocate) an allocated Macro
 * @param mcr the macro to free
 */
void freeMcr(Macro *mcr) {
    free(mcr->name);
    free(mcr);
}

/**
 * Free a macro list
 * @param head the head of the list to free
 */
void freeMcrList(Macro *head) {
    if (head == NULL)
        return;
    
    freeMcrList(head->next);
    freeMcr(head);
}
