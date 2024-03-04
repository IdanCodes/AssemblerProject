#include <stdlib.h>
#include "first_stage.h"
#include "../utils/inpututils.h"
#include "../utils/fileutils.h"
#include "../utils/strutils.h"
#include "../utils/charutils.h"
#include "../utils/logger.h"
#include "../utils/strutils.h"

static void printFirstStageError(enum firstStageErr err, unsigned int sourceLine, ...); /* TODO: implement */
static int getSymbolByName(char *name, Symbol *head, Symbol **pSymbol);
static void registerConstant(Symbol **head, char *name, double value);
static enum firstStageErr fetchConstant(char *line, Symbol **constants);

/* fileName is the file's name without the extension */
void assemblerFirstStage(char fileName[]) {
    /* -- declarations -- */
    unsigned int instructionCounter, dataCounter, sourceLine, skippedLines;
    char sourceFileName[FILENAME_MAX]/*, outFileName[FILENAME_MAX]*/, *token;
    char line[MAXLINE + 1]; /* account for '\0' */
    int len;
    FILE *sourcef;
    Symbol *symbols;
    enum firstStageErr err;
    
    
    /* -- open files -- */
    sprintf(sourceFileName, "%s.%s", fileName, PRE_ASSEMBLED_FILE_EXTENSION);
    /* TODO: out file? sprintf(outFileName, "%s.%s", fileName, OBJECT_FILE_EXTENSION); */
    
    openFile(sourceFileName, "r", &sourcef);
    
    
    /* -- main loop -- */
    instructionCounter = 0;
    dataCounter = 0;
    sourceLine = 0;
    symbols = NULL;
    while ((skippedLines = getNextLine(sourcef, line, MAXLINE, &len)) != getLine_FILE_END) {
        sourceLine += skippedLines;
        token = getStart(line);
        
        /* check if the line is a constant definition */
        if (tokcmp(token, KEYWORD_CONSTANT_DEFINITION) == 0) {
            if ((err = fetchConstant(line, &symbols)) != firstStageErr_no_err)
                printFirstStageError(err, sourceLine);
            continue;
        }
    }
}

static void printFirstStageError(enum firstStageErr err, unsigned int sourceLine, ...) {
    /* TODO: implement & print the file where the error came from */
    printf("ERROR %d in line %u (.am)\n", err, sourceLine);
}

/* is the token a valid name for a label? */
int validSymbolName(char *name, Symbol *head) {
    int len;
    Symbol *tempS;
    
    if (*name == '\0')
        return 0;
    
    if (getSymbolByName(name, head, &tempS))
        return 0;   /* a symbol with this name already exists */
    
    if (isnumber(*name))    /* is the first character a number? */
        return 0;
        
    for (len = 1, name++; len < LABEL_MAX_SIZE && isalnum(*name); len++, name++)
        ; /* continue reading the name as long as it is alphanumeric */

    /* if we've reached the end of the string, it means it's completely alphanumeric (except the first character) */
    return len <= LABEL_MAX_SIZE && (isspace(*name) || *name == '\0');
}

static int getSymbolByName(char *name, Symbol *head, Symbol **pSymbol) {
    while (head != NULL) {
        if (tokcmp(head->name, name) == 0) {
            *pSymbol = head;
            return 1;
        }
        head = head->next;
    }
    
    return 0;
}

static void registerConstant(Symbol **head, char *name, double value) {
    Symbol *pConst, *temp;

    pConst = (Symbol *)malloc(sizeof(Symbol));
    if (pConst == NULL)
        terminalError(1, "Insufficient memory\n");

    pConst->name = strdup(name);
    if (pConst->name == NULL)
        terminalError(1, "Insufficient memory\n");

    pConst->value = (union SymbolType *)malloc(sizeof(double));
    if (pConst->value == NULL)
        terminalError(1, "Insufficient memory\n");

    pConst->value->value = value;
    
    if (*head == NULL) {
        *head = pConst;
        return;
    }
    
    temp = *head;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = pConst;
    pConst->mdefine = 1;
}

static enum firstStageErr fetchConstant(char *line, Symbol **constants) {
    char *name, *token, *equalsSign, *strVal;
    double doubleVal;
    Symbol *tempConst;
    
    token = getStart(line);
    
    /* fetch name + syntax validation */
    name = (token = getNextToken(token));
    equalsSign = getFirstOrEnd(token, '=');
    
    if (name == equalsSign) /* equals sign appeared before name */
        return firstStageErr_name_expected_define;
    
    if (getNextToken(name) < equalsSign)    /* equal sign expected */
        return firstStageErr_expected_equal_sign_define;

    /* fetch strVal + syntax validation */
    strVal = (token = getNextToken(equalsSign));
    if (*getNextToken(token) != '\0')   /* extra characters */
        return firstStageErr_unexpected_chars_define;

    /* make sure the constant's name is valid */
    if (!validLabelName(name))
        return firstStageErr_invalid_name_define;

    /* check name against existing constants' names */
    if (*constants != NULL && getSymbolByName(name, *constants, &tempConst))
        return firstStageErr_name_taken_define;

    /* TODO: check against saved keywords */

    /* make sure the value is a number */
    if (!tryParseNumber(strVal, &doubleVal))
        return firstStageErr_value_nan_define;

    /* register the new constant */
    registerConstant(constants, name, doubleVal);
    logInfo("Registered constant '%s': %g.\n", name, doubleVal);
    
    return firstStageErr_no_err;
}
