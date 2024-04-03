#include "second_stage.h"
#include "first_stage.h"
#include "../utils/inpututils.h"
#include "../utils/fileutils.h"
#include "../utils/strutils.h"
#include "../utils/charutils.h"
#include "../utils/keywords.h"
#include "../utils/logger.h"

static void printSecondStageErr(enum secondStageErr err, unsigned int lineNumber, char *fileName);
static char *getErrMsg(enum secondStageErr err);

int assemblerSecondStage(char fileName[], int *data, Symbol *symbols, ByteNode *bytes, int numInstructions, int dataCounter) {
    char *token, *tokEnd;
    char sourceFileName[FILENAME_MAX], objFileName[FILENAME_MAX], entFileName[FILENAME_MAX], extFileName[FILENAME_MAX];
    char line[MAXLINE + 1];
    unsigned int sourceLine, skippedLines;
    int instructionCounter, len, hasErr;
    FILE *sourcef, *objf, *entf, *extf;
    Symbol *tempSym;
    
    
    /* -- open files -- */
    sprintf(sourceFileName, "%s.%s", fileName, PRE_ASSEMBLED_FILE_EXTENSION);
    sprintf(objFileName, "%s.%s", fileName, OBJECT_FILE_EXTENSION);
    sprintf(entFileName, "%s.%s", fileName, ENTRIES_FILE_EXTENSION);
    sprintf(extFileName, "%s.%s", fileName, EXTERNALS_FILE_EXTENSION);

    openFile(sourceFileName, "r", &sourcef);
    openFile(objFileName, "w", &objf);
    openFile(entFileName, "w", &entf);
    openFile(extFileName, "w", &extf);
    
    sourceLine = 0;
    instructionCounter = 0;
    hasErr = 0;
    while ((skippedLines = getNextLine(sourcef, line, MAXLINE, &len)) != getLine_FILE_END) {
        sourceLine += skippedLines;
        
        token = getStart(line);
        
        /* label declaration? */
        if (*(tokEnd = getTokEnd(token)) == LABEL_END_CHAR)
            token = getNextToken(token);    /* skip to next token */

        /* TODO: getNumWords(Operation op, int sourceAddr, int destAddr) in operations.c - returns the number of binary words the operation takes */
        /* .data, .string or .extern instruction? */
        if (tokcmp(token, KEYWORD_DATA_DEC) == 0 ||
            tokcmp(token, KEYWORD_STRING_DEC) == 0 ||
            tokcmp(token, KEYWORD_EXTERN_DEC) == 0)
            continue;   /* read next line */
        
        /* .entry instruction? */
        if (tokcmp(token, KEYWORD_ENTRY_DEC) == 0) {
            /* TODO: write to entry file */
            token = getNextToken(token);
            *(getTokEnd(token) + 1) = '\0'; /* this line will not be read anymore - no need to save the character's value */
            if (!getSymbolByName(token, symbols, &tempSym)) {
                printSecondStageErr(secondStageErr_entry_undefined, sourceLine, sourceFileName);
                hasErr = 1;
                continue;
            }
            
            /* was this symbol already declared as entry? */
            if ((tempSym->flag & SYMBOL_FLAG_ENTRY) != 0) {
                logWarn("label '%s' was already declared as entry (in file \"%s\", line %u)\n", token, sourceFileName, sourceLine);
                continue;   /* skip to the next line */
            }

            /* TODO: remove this log */
            logInfo("Entry '%s' in line %u in file '%s'\n", token, sourceLine, sourceFileName);
            tempSym->flag |= SYMBOL_FLAG_ENTRY; /* declare symbol as entry */
            fprintf(entf, "%d %s\n", tempSym->value, token);
            continue;
        }
    }
    
    return hasErr;
}

static void printSecondStageErr(enum secondStageErr err, unsigned int lineNumber, char *fileName) {
    char *message;

    if (err == secondStageErr_no_err)
        return;

    message = getErrMsg(err);
    logErr("file \"%s\" line %u - %s\n", fileName, lineNumber, message);
}

static char *getErrMsg(enum secondStageErr err) {
    switch (err) {
        case secondStageErr_entry_undefined:
            return "undefined label";
        
        default:
            return "UNDEFINED ERROR";
    }
}
