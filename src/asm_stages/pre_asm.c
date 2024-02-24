#include <stdlib.h>
#include "pre_asm.h"
#include "../utils/logger.h"
#include "../utils/inpututils.h"
#include "../utils/strutils.h"
#include "../utils/fileutils.h"
#include "../utils/charutils.h"

/* DOCUMENT preAssemble */
/* fileName is the name of the file without the added extension */
enum preAssembleStatus preAssemble(char fileName[]) {
    /* -- declarations -- */
    unsigned int sourceLine;
    int skippedLines, len, readingMcr;
    char line[MAXLINE + 1];   /* account for '\0' */
    char sourceFileName[FILENAME_MAX], outFileName[FILENAME_MAX], *token;
    FILE *sourcef, *outf;
    MACRO *head, *tail, *mcr;
    enum preAssembleErr preAsmErr;
    
    /* -- open source and output files -- */
    sprintf(sourceFileName, "%s.%s", fileName, SOURCE_FILE_EXTENSION);
    sprintf(outFileName, "%s.%s", fileName, PRE_ASSEMBLED_FILE_EXTENSION);

    openFile(sourceFileName, "r", &sourcef);    /* open requested file for reading */
    openFile(outFileName, "w", &outf);      /* open pre-assembled file for writing */
    
    
    /* -- main loop -- */
    sourceLine = 0;
    head = NULL;
    preAsmErr = preAssembleErr_no_err;
    readingMcr = 0;
    while ((skippedLines = getNextLine(sourcef, line, MAXLINE, &len)) != getLine_FILE_END) {
        sourceLine += skippedLines;
        
        token = line;   /* line is already trimmed */
        if (readingMcr) {
            if (tokcmp(token, END_MCR_INSTRUCTION) == 0) {
                readingMcr = 0;

                /* error handling */
                if (*getNextToken(token) != '\0') {
                    preAsmErr = preAssembleErr_unexpected_chars_end;
                    break;
                }

                tail->endLine = sourceLine - 1; /* tail's end because there can't be nested macros */
            }
            continue; /* skip print */
        }

        if (tokcmp(token, DEC_MCR_INSTRUCTION) == 0) {
            readingMcr = 1;

            /* error handling */
            if (*(token = getNextToken(token)) == '\0') {
                preAsmErr = preAssembleErr_mcr_expected;
                break;
            }
            if (*getNextToken(token) != '\0') {
                preAsmErr = preAssembleErr_unexpected_chars_dec;
                break;
            }
            if (getMacroWithName(token, head) != NULL) {
                preAsmErr = preAssembleErr_macro_exists;
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
                preAsmErr = preAssembleErr_unexpected_chars_ref;
                break;
            }
            
            logInfo("Expanding '%s' in line %u\n", token, sourceLine);
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

    if (preAsmErr != preAssembleErr_no_err) {   /* TODO: use logerror instead */
        printf("%s (line %u).\n", preAsmErrMessage(preAsmErr), sourceLine);
        deleteFile(outFileName);
    }
    
    return preAssemble_OK;
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
        case preAssembleErr_unexpected_chars_ref:
            return "Unexpected characters after macro call";
        case preAssembleErr_macro_exists:
            return "Macro name already used";
            
        default:
            return "";
    }
}

/**
 * Allocate a MACRO
 * @param name the name of the macro
 * @return the address of the allocated macro
 */
MACRO *allocMcr(char *name) {
    MACRO *result;
    
    result = (MACRO *)malloc(sizeof(MACRO));
    if (result == NULL)
        terminalError(1, "Insufficient memory");
    
    result->name = strdup(name);
    if (result->name == NULL)
        terminalError(1, "Insufficient memory");
    
    result->next = NULL;
    
    return result;
}

/**
 * Free (deallocate) an allocated MACRO
 * @param mcr the macro to free
 */
void freeMcr(MACRO *mcr) {
    free(mcr->name);
    free(mcr);
}

/**
 * Check if a macro with the given name exists (in the given list)
 * @param name the name to search
 * @param head the head of the list to search in
 * @return if the macro was found, returns its address. otherwise returns NULL.
 */
MACRO *getMacroWithName(char *name, MACRO *head) {
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
void expandMacro(MACRO *mcr, FILE *destf, char *sourcefileName) {
    unsigned int sourceLine;
    int len;
    char line[MAXLINE + 1];
    FILE *sourcef;
    
    openFile(sourcefileName, "r", &sourcef);
    
    for (sourceLine = 0; sourceLine < mcr->startLine; sourceLine++) {
        while (getc(sourcef) != '\n')
            ;   /* read a sourceLine */
    }
    
    for (sourceLine = mcr->startLine; sourceLine <= mcr->endLine;) {
        sourceLine += getNextLine(sourcef, line, MAXLINE, &len);
        fprintf(destf, "%s\n", line);
    }
    
    fclose(sourcef);
}
