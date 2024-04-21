#include "pre_asm.h"
#include "../utils/logger.h"
#include "../utils/inpututils.h"
#include "../utils/strutils.h"
#include "../utils/fileutils.h"
#include "../utils/charutils.h"
#include "../utils/keywords.h"

static void printPreAsmErr(enum preAssembleErr err, unsigned int sourceLine, char *sourceFileName);
static char *preAsmErrMessage(enum preAssembleErr err);

/* DOCUMENT preAssemble */
/* fileName is the name of the file without the added extension */
/* returns 0 if there was an error (at least one), else returns 1 */
int preAssemble(char fileName[FILENAME_MAX], Macro **macros) {
    /* -- declarations -- */
    unsigned int sourceLine, skippedLines;
    int readingMcr, hasErr;
    char line[MAXLINE + 1]; /* account for '\0' */
    char sourceFileName[FILENAME_MAX], outFileName[FILENAME_MAX], *token;
    FILE *sourcef, *outf;
    Macro *tail, *mcr;
    enum getLineStatus lineStatus;
    
    /* -- open source and output files -- */
    sprintf(sourceFileName, "%s.%s", fileName, SOURCE_FILE_EXTENSION);
    sprintf(outFileName, "%s.%s", fileName, PRE_ASSEMBLED_FILE_EXTENSION);

    /* open requested file for reading */
    if (!tryOpenFile(sourceFileName, "r", &sourcef)) {
        logErr("Insufficient permissions/storage to open file '%s', or it doesn't exist.\n", sourceFileName);
        return 1;
    }
        
    openFile(outFileName, "w", &outf);      /* open pre-assembled file for writing */
    
    
    /* -- main loop -- */
    sourceLine = 0;
    readingMcr = 0;
    hasErr = 0;
    while ((lineStatus = getNextLine(sourcef, line, 0, MAXLINE, &skippedLines)) != getLine_FILE_END) {
        sourceLine += skippedLines;
        if (lineStatus == getLine_TOO_LONG) {
            logWarn("Line %u in file \"%s\" is too long - ignoring it (maximum length is %d characters).\n", sourceLine, sourceFileName, MAXLINE - 1);
            continue;
        }
        
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

                tail->endLine = sourceLine; /* tail's the end because there can't be nested macros */
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
            /* is the macro's name specified? */
            if (*(token = getNextToken(token)) == '\0') {
                printPreAsmErr(preAssembleErr_mcr_expected, sourceLine, sourceFileName);
                hasErr = 1;
                break;
            }
            
            /* are there unexpected characters after the macro's name? */
            if (*getNextToken(token) != '\0') {
                printPreAsmErr(preAssembleErr_unexpected_chars_dec, sourceLine, sourceFileName);
                hasErr = 1;
                break;
            }
            
            /* is the macro's name valid? */
            if (!validMcrName(token)) {
                printPreAsmErr(preAssembleErr_macro_invalid_name, sourceLine, sourceFileName);
                hasErr = 1;
                break;
            }
            
            /* is there another macro with the same name? */
            if (getMacroWithName(token, *macros) != NULL) {
                printPreAsmErr(preAssembleErr_macro_exists, sourceLine, sourceFileName);
                hasErr = 1;
                break;
            }
            
            /* is the macro's name a saved keyword? */
            if (isSavedKeyword(token)) {
                printPreAsmErr(preAssembleErr_macro_saved_name, sourceLine, sourceFileName);
                hasErr = 1;
                break;
            }

            /* add the macro to the list */
            if (*macros == NULL)
                tail = (*macros = allocMcr(token));
            else
                tail = (tail->next = allocMcr(token));

            tail->startLine = sourceLine + 1; /* at least another line is guaranteed (endmcr) */
            continue; /* skip print */
        }

        if ((mcr = getMacroWithName(token, *macros)) != NULL) {
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

    if (hasErr)
        deleteFile(outFileName);
    
    return hasErr;
}

static void printPreAsmErr(enum preAssembleErr err, unsigned int sourceLine, char *sourceFileName) {
    logErr("%s (line %u in file '%s')\n", preAsmErrMessage(err), sourceLine, sourceFileName);
}

/**
 * Get the error message that matches a preAssembleErr
 * @param err the given preAssembleErr
 * @return a string representing the error of the given preAssembleErr
 */
static char *preAsmErrMessage(enum preAssembleErr err) {
    switch (err) {
        case preAssembleErr_mcr_expected:
            return "macro name expected";
            
        case preAssembleErr_unexpected_chars_dec:
            return "unexpected characters after macro declaration";
            
        case preAssembleErr_unexpected_chars_end:
            return "unexpected characters after macro end";
            
        case preAssembleErr_unexpected_chars_call:
            return "unexpected characters after macro call";
            
        case preAssembleErr_macro_exists:
            return "macro name already used";
            
        case preAssembleErr_macro_saved_name:
            return "macro name is a saved keyword";
            
        case preAssembleErr_unexpected_end:
            return "macro end is outside of a macro definition";
            
        case preAssembleErr_macro_invalid_name:
            return "invalid name for a macro";
            
        default:
            return "UNDEFINED ERROR";
    }
}
