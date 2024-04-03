#include "second_stage.h"
#include "../utils/inpututils.h"
#include "../utils/fileutils.h"
#include "../utils/strutils.h"
#include "../utils/charutils.h"
#include "../utils/keywords.h"
#include "../utils/logger.h"

int assemblerSecondStage(char fileName[], int *data, Symbol *symbols, ByteNode *bytes, int numInstructions, int dataCounter) {
    char *token, *tokEnd;
    char sourceFileName[FILENAME_MAX], objFileName[FILENAME_MAX], entFileName[FILENAME_MAX], extFileName[FILENAME_MAX];
    char line[MAXLINE + 1];
    unsigned int sourceLine, skippedLines;
    int instructionCounter, len;
    FILE *sourcef, *objf, *entf, *extf;
    
    
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
            *(getTokEnd(token) + 1) = '\0';
            logInfo("Entry '%s' in line %u in file '%s'\n", token, sourceLine, sourceFileName);
            continue;
        }
    }
    
    return 0;
}
