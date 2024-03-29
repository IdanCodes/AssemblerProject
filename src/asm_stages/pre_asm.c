#include <stdlib.h>
#include "pre_asm.h"
#include "../utils/logger.h"
#include "../utils/inpututils.h"
#include "../utils/strutils.h"
#include "../utils/fileutils.h"
#include "../utils/charutils.h"
#include "../utils/keywords.h"

static void printPreAsmErr(enum preAssembleErr err, unsigned int sourceLine, char *sourceFileName);

/* DOCUMENT preAssemble */
/* fileName is the name of the file without the added extension */
/* returns 1 if there were no erros, else returns 0 */
enum preAssembleErr preAssemble(char fileName[]) {
    /* -- declarations -- */
    unsigned int sourceLine;
    int skippedLines, len, readingMcr, hasErr;
    char line[MAXLINE + 1]; /* account for '\0' */
    char sourceFileName[FILENAME_MAX], outFileName[FILENAME_MAX], *token;
    FILE *sourcef, *outf;
    Macro *head, *tail, *mcr;
    
    /* -- open source and output files -- */
    sprintf(sourceFileName, "%s.%s", fileName, SOURCE_FILE_EXTENSION);
    sprintf(outFileName, "%s.%s", fileName, PRE_ASSEMBLED_FILE_EXTENSION);

    openFile(sourceFileName, "r", &sourcef);    /* open requested file for reading */
    openFile(outFileName, "w", &outf);      /* open pre-assembled file for writing */
    
    
    /* -- main loop -- */
    sourceLine = 0;
    head = NULL;
    readingMcr = 0;
    hasErr = 0;
    while ((skippedLines = getNextLine(sourcef, line, MAXLINE, &len)) != getLine_FILE_END) {
        sourceLine += skippedLines;
        
        token = line;   /* line is already trimmed */
        if (readingMcr) {
            if (tokcmp(token, KEYWORD_MCR_END) == 0) {
                readingMcr = 0;

                /* error handling */
                if (*getNextToken(token) != '\0') {
                    printPreAsmErr(preAssembleErr_unexpected_chars_end, sourceLine, sourceFileName);
                    hasErr = 1;
                    break;
                }

                tail->endLine = sourceLine - 1; /* tail's the end because there can't be nested macros */
            }
            continue; /* skip print */
        }
        else if (tokcmp(token, KEYWORD_MCR_END) == 0) {
            printPreAsmErr(preAssembleErr_unexpected_end, sourceLine, sourceFileName);
            hasErr = 1;
            break;
        }

        if (tokcmp(token, KEYWORD_MCR_DEC) == 0) {
            readingMcr = 1;

            /* error handling */
            if (*(token = getNextToken(token)) == '\0') {
                printPreAsmErr(preAssembleErr_mcr_expected, sourceLine, sourceFileName);
                hasErr = 1;
                break;
            }
            
            if (*getNextToken(token) != '\0') {
                printPreAsmErr(preAssembleErr_unexpected_chars_dec, sourceLine, sourceFileName);
                hasErr = 1;
                break;
            }
            
            if (getMacroWithName(token, head) != NULL) {
                printPreAsmErr(preAssembleErr_macro_exists, sourceLine, sourceFileName);
                hasErr = 1;
                break;
            }
            
            if (isSavedKeyword(token)) {
                printPreAsmErr(preAssembleErr_macro_saved_name, sourceLine, sourceFileName);
                hasErr = 1;
                break;
            }

            /* add the macro to the list */
            if (head == NULL)
                tail = (head = allocMcr(token));
            else
                tail = (tail->next = allocMcr(token));

            tail->startLine = sourceLine + 1; /* at least another line is guaranteed (endmcr) */
            continue; /* skip print */
        }

        if ((mcr = getMacroWithName(token, head)) != NULL) {
            if (*getNextToken(token) != '\0') {
                printPreAsmErr(preAssembleErr_unexpected_chars_call, sourceLine, sourceFileName);
                hasErr = 1;
                break;
            }
            
            expandMacro(mcr, outf, sourceFileName);
            continue;
        }
        
        fprintf(outf, "%s\n", line);
    }
    
    
    /* -- cleaning up -- */
    fclose(sourcef);
    fclose(outf);
    
    while (head != NULL) {
        freeMcr(head);
        head = head->next;
    }

    if (hasErr)
        deleteFile(outFileName);
    
    return !hasErr;
}

static void printPreAsmErr(enum preAssembleErr err, unsigned int sourceLine, char *sourceFileName) {
    logErr("%s (line %u in file '%s').\n", preAsmErrMessage(err), sourceLine, sourceFileName);
}

/**
 * Get the error message that matches a preAssembleErr
 * @param err the given preAssembleErr
 * @return a string representing the error of the given preAssembleErr
 */
char *preAsmErrMessage(enum preAssembleErr err) {
    switch (err) {
        case preAssembleErr_mcr_expected:
            return "Macro name expected";
            
        case preAssembleErr_unexpected_chars_dec:
            return "Unexpected characters after macro declaration";
            
        case preAssembleErr_unexpected_chars_end:
            return "Unexpected characters after macro end";
            
        case preAssembleErr_unexpected_chars_call:
            return "Unexpected characters after macro call";
            
        case preAssembleErr_macro_exists:
            return "Macro name already used";
            
        case preAssembleErr_macro_saved_name:
            return "Macro name is a saved keyword";
            
        case preAssembleErr_unexpected_end:
            return "Macro end is outside of a macro definition";
            
        default:
            return "UNDEFINED ERROR";
    }
}

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
