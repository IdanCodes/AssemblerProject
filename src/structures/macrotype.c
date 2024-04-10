#include <stdlib.h>
#include <string.h>
#include "macrotype.h"
#include "../utils/logger.h"
#include "../utils/strutils.h"
#include "../utils/inpututils.h"
#include "../utils/fileutils.h"

/**
 * Allocate a Macro
 * @param name the name of the macro
 * @return the address of the allocated macro
 */
Macro *allocMcr(char *name) {
    Macro *result;

    result = (Macro *)malloc(sizeof(Macro));
    if (result == NULL)
        logInsuffMemErr("allocating macro");

    result->name = strdup(name);
    if (result->name == NULL)
        logInsuffMemErr("allocating macro's name");

    result->next = NULL;

    return result;
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
    int len;
    char line[MAXLINE + 1];
    FILE *sourcef;

    openFile(sourcefileName, "r", &sourcef);

    for (sourceLine = 0, skippedLines = 0; sourceLine < mcr->startLine-1; sourceLine += skippedLines) {
        if ((skippedLines = getNextLine(sourcef, line, MAXLINE, &len)) == getLine_FILE_END)
            terminalError(1, "Error expanding macro '%s' - reached end of file (in file '%s').\n", mcr->name, sourcefileName);
    }

    sourceLine += getNextLine(sourcef, line, MAXLINE, &len);
    for (; sourceLine < mcr->endLine;) {
        fprintf(destf, "%s\n", line);
        sourceLine += getNextLine(sourcef, line, MAXLINE, &len);
    }

    fclose(sourcef);
}
