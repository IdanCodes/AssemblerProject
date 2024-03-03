#include <stdlib.h>
#include "first_stage.h"
#include "../utils/inpututils.h"
#include "../utils/fileutils.h"
#include "../utils/strutils.h"
#include "../utils/charutils.h"
#include "../utils/logger.h"
#include "../utils/strutils.h"

static void printFirstStageError(enum firstStageErr err, unsigned int sourceLine, ...); /* TODO: implement */
static int constWithNameExists(char *name, AsmConst *head, AsmConst **c);
static void registerConstant(AsmConst **head, char *name, double value);
static enum firstStageErr fetchConstant(char *line, AsmConst **constants);

/* fileName is the file's name without the extension */
void assemblerFirstStage(char fileName[]) {
    /* -- declarations -- */
    unsigned int instructionCounter, dataCounter, sourceLine, skippedLines;
    char sourceFileName[FILENAME_MAX]/*, outFileName[FILENAME_MAX]*/, *token;
    char line[MAXLINE + 1]; /* account for '\0' */
    int len;
    FILE *sourcef;
    AsmConst *constants;
    enum firstStageErr err;
    
    
    /* -- open files -- */
    sprintf(sourceFileName, "%s.%s", fileName, PRE_ASSEMBLED_FILE_EXTENSION);
    /* TODO: out file? sprintf(outFileName, "%s.%s", fileName, OBJECT_FILE_EXTENSION); */
    
    openFile(sourceFileName, "r", &sourcef);
    
    
    /* -- main loop -- */
    instructionCounter = 0;
    dataCounter = 0;
    sourceLine = 0;
    constants = NULL;
    while ((skippedLines = getNextLine(sourcef, line, MAXLINE, &len)) != getLine_FILE_END) {
        sourceLine += skippedLines;
        token = getStart(line);
        
        /* check if the line is a constant definition */
        if (tokcmp(token, KEYWORD_CONSTANT_DEFINITION) == 0) {
            if ((err = fetchConstant(line, &constants)) != firstStageErr_no_err)
                printFirstStageError(err, sourceLine);
            continue;
        }
    }
}

static void printFirstStageError(enum firstStageErr err, unsigned int sourceLine, ...) {
    /* TODO: implement & print the file where the error came from */
    printf("ERROR - %d\n", err);
}

int validLabelName(char *name) {
    int len;
    if (*name == '\0')
        return 0;
    
    if (isnumber(*name))    /* is the first character a number? */
        return 0;
        
    for (len = 1, name++; len < LABEL_MAX_SIZE && isalnum(*name); len++, name++)
        ; /* continue reading the name as long as it is alphanumeric */

    /* if we've reached the end of the string, it means it's completely alphanumeric (except the first character) */
    return len <= LABEL_MAX_SIZE && (isspace(*name) || *name == '\0');
}

static int constWithNameExists(char *name, AsmConst *head, AsmConst **c) {
    while (head != NULL) {
        if (tokcmp(head->name, name) == 0) {
            *c = head;
            return 1;
        }
        head = head->next;
    }
    
    return 0;
}

static void registerConstant(AsmConst **head, char *name, double value) {
    AsmConst *c, *temp;
    
    c = (AsmConst *)malloc(sizeof(AsmConst));
    if (c == NULL)
        terminalError(1, "Insufficient memory");
    
    c->name = strdup(name);
    if (c->name == NULL)
        terminalError(1, "Insufficient memory");
    
    c->value = value;
    
    if (*head == NULL) {
        *head = c;
        return;
    }
    
    temp = *head;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = c;
}

static enum firstStageErr fetchConstant(char *line, AsmConst **constants) {
    char *name, *token, *equalsSign, *strVal;
    double doubleVal;
    AsmConst *tempConst;
    
    token = getStart(line);
    
    /* fetch name + syntax validation */
    name = (token = getNextToken(token));
    equalsSign = getFirstOrEnd(token, '=');
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
    if (*constants != NULL && constWithNameExists(name, *constants, &tempConst))
        return firstStageErr_name_taken_define;

    /* TODO: check against saved keywords */

    /* make sure the value is a number */
    if (!tryParseNumber(strVal, &doubleVal))
        return firstStageErr_const_value_nan;

    /* register the new constant */
    registerConstant(constants, name, doubleVal);
    logInfo("Registered constant '%s': %g.\n", name, doubleVal);
    
    return firstStageErr_no_err;
}
