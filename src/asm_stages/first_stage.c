#include <stdlib.h>
#include "first_stage.h"
#include "../utils/inpututils.h"
#include "../utils/fileutils.h"
#include "../utils/strutils.h"
#include "../utils/charutils.h"
#include "../utils/logger.h"
#include "../utils/keywords.h"

static void freeSymbolsList(Symbol *head);
static void printFirstStageError(enum firstStageErr err, unsigned int sourceLine, ...); /* TODO: implement */
static int getSymbolByName(char *name, Symbol *head, Symbol **pSymbol);
static void registerConstant(Symbol **head, char *name, int value);
static enum firstStageErr fetchConstant(char *line, Symbol **constants);
static enum firstStageErr storeDataArgs(char *token, int *dataCounter, Symbol *symbols, int **data);
static enum firstStageErr fetchData(char *lblName, char *token, int *dataCounter, Symbol **symbols, int **data);
static enum firstStageErr fetchNumber(char *start, char *end, int *num, Symbol *symbols);
static void addSymToList(Symbol **head, Symbol *symbol);

/* DOCUMENT */
/* fileName is the file's name without the extension */
void assemblerFirstStage(char fileName[]) {
    /* -- declarations -- */
    unsigned int sourceLine, skippedLines;
    int dataCounter/*, instructionCounter*/;
    char sourceFileName[FILENAME_MAX]/*, outFileName[FILENAME_MAX]*/, *labelName, *token, *tokEnd;
    char line[MAXLINE + 1]; /* account for '\0' */
    int len, labelDefinition, i, *data;
    FILE *sourcef;
    Symbol *symbols, *tempSymb;
    enum firstStageErr err;
    
    
    /* -- open files -- */
    sprintf(sourceFileName, "%s.%s", fileName, PRE_ASSEMBLED_FILE_EXTENSION);
    /* TODO: out file? sprintf(outFileName, "%s.%s", fileName, OBJECT_FILE_EXTENSION); */
    
    openFile(sourceFileName, "r", &sourcef);
    
    
    /* -- main loop -- */
    /*instructionCounter = 0;*/
    dataCounter = 0;
    data = (int *)malloc(sizeof(int));
    if (data == NULL)
        terminalError(1, "Insufficient memory\n");
    
    sourceLine = 0;
    symbols = NULL;
    while ((skippedLines = getNextLine(sourcef, line, MAXLINE, &len)) != getLine_FILE_END) {
        labelDefinition = 0;
        sourceLine += skippedLines;
        token = getStart(line);
        
        /* label declaration? */
        if (*(tokEnd = getTokEnd(token)) == LABEL_END_CHAR) {
            if (!validSymbolName(token, tokEnd - 1)) {
                printFirstStageError(firstStageErr_invalid_name_label, sourceLine);
                continue;
            }
            
            /* save label name */
            labelName = (char *)malloc(sizeof(char [LABEL_MAX_LENGTH]));
            if (labelName == NULL)
                terminalError(1, "Insufficient memory\n");
            
            for (i = 0; i < tokEnd - token; i++)
                labelName[i] = token[i];
            labelName[i] = '\0';
            
            if (isSavedKeyword(labelName)) {
                printFirstStageError(firstStageErr_saved_keyword_label, sourceLine);
                continue;
            }
            
            if (getSymbolByName(labelName, symbols, &tempSymb)) {
                printFirstStageError(firstStageErr_name_taken_label, sourceLine);
                continue;
            }
            
            labelDefinition = 1;
            token = getNextToken(token);
            
            /* empty line */
            if (*token == '\0') {
                printFirstStageError(firstStageErr_label_empty_line, sourceLine);
                continue;
            }
        }

        /* constant declaration? */
        if (tokcmp(token, KEYWORD_CONST_DEC) == 0) {
            if (labelDefinition)
                printFirstStageError(firstStageErr_const_defined_in_label, sourceLine);
            else if ((err = fetchConstant(line, &symbols)) != firstStageErr_no_err)
                printFirstStageError(err, sourceLine);
            continue;
        }
        
        /* storage instruction? */
        if (tokcmp(token, KEYWORD_DATA_DEC) == 0) {
            if ((err = fetchData(labelDefinition ? labelName : "", token, &dataCounter, &symbols, &data)) != firstStageErr_no_err)
                printFirstStageError(err, sourceLine);
            if (labelDefinition)
                logInfo("Added data '%s' - new data counter %d\n", labelName, dataCounter);
            else
                logInfo("Added unnamed data - new data counter %d\n", dataCounter);
            continue;
        }
        
        if (tokcmp(token, KEYWORD_STRING_DEC) == 0) {
            
        }
    }

    free(data);
    freeSymbolsList(symbols);
}

static void printFirstStageError(enum firstStageErr err, unsigned int sourceLine, ...) {
    /* TODO: implement & print the file where the error came from */
    printf("ERROR %d in line %u (.am)\n", err, sourceLine);
}

/* is the token a valid name for a symbol */
int validSymbolName(char *tok, char *end) {
    if (end <= tok || (end - tok) >= LABEL_MAX_LENGTH ||  /* invalid length */
        !isalpha(*tok))    /* first character is not alphabetic */
        return 0;

    for (tok++; tok < end; tok++) {
        if (!isalnum(*tok))
            return 0;
    }

    return 1;
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

static void registerConstant(Symbol **head, char *name, int value) {
    char temp, *end;
    Symbol *pConst;

    pConst = (Symbol *)malloc(sizeof(Symbol));
    if (pConst == NULL)
        terminalError(1, "Insufficient memory\n");
    
    temp = *(end = (getTokEnd(name) + 1));
    *end = '\0'; /* to use strdup */
    
    pConst->name = strdup(name);
    if (pConst->name == NULL)
        terminalError(1, "Insufficient memory\n");
    
    *end = temp;

    pConst->value = value;
    pConst->mdefine = 1;
    
    addSymToList(head, pConst);
}

static enum firstStageErr fetchConstant(char *line, Symbol **constants) {
    char *name, *token, *equalsSign, *strVal, tempC;
    int numberValue;
    Symbol *tempSymbol;
    
    token = getStart(line);
    
    /* fetch name + syntax validation */
    name = (token = getNextToken(token));
    equalsSign = getFirstOrEnd(token, '=');
    
    if (name == equalsSign) /* equals sign appeared before name */
        return firstStageErr_name_expected_define;
    
    if (*equalsSign == '\0' || getNextToken(name) < equalsSign)    /* equal sign expected */
        return firstStageErr_expected_equal_sign_define;

    /* fetch strVal + syntax validation */
    strVal = (token = getNextToken(equalsSign));
    if (*strVal == '\0')
        return firstStageErr_value_expected_define;
    
    if (*getNextToken(token) != '\0')   /* extra characters */
        return firstStageErr_unexpected_chars_define;

    /* make sure the constant's name is valid */
    if (!validSymbolName(name, getTokEnd(name)))
        return firstStageErr_invalid_name_define;

    /* check if the name is a saved keyword */
    if (isSavedKeyword(name))
        return firstStageErr_saved_keyword_define;
    
    /* check if the name is an existing constants' names */
    if (*constants != NULL && getSymbolByName(name, *constants, &tempSymbol))
        return firstStageErr_name_taken_define;

    /* make sure the numberValue is a number */
    if (!tryParseToken(strVal, &numberValue))
        return firstStageErr_value_nan_define;

    /* register the new constant */
    registerConstant(constants, name, numberValue);
    
    /* TODO: remove this debug log */
    tempC = *(getTokEnd(name) + 1) = '\0';
    logInfo("Registered constant '%s': %d.\n", name, numberValue);
    *(getTokEnd(name) + 1) = tempC;
    
    return firstStageErr_no_err;
}

static void freeSymbolsList(Symbol *head) {
    if (head == NULL)
        return;
    
    freeSymbolsList(head->next);
    free(head->name);
    free(head);
}

/* token is the first number parameter */
static enum firstStageErr storeDataArgs(char *token, int *dataCounter, Symbol *symbols, int **data) {
    int num;
    char *end;
    enum firstStageErr err;
    
    token = getStart(token);
    while (*token != '\0') {
        end = getFirstOrEnd(token, ',');
        
        if (*end != '\0' && getTokEnd(token) < end)
            return firstStageErr_data_comma_expected;

        if ((err = fetchNumber(token, end, &num, symbols)) != firstStageErr_no_err)
            return err;
        
        token = getNextToken(end);
        
        /* store the number into memory and increment data counter */
        *data = (int *)realloc(*data, sizeof(int) * (*dataCounter));
        if (*data == NULL)
            terminalError(1, "Insufficient memory\n");
        
        (*data)[*dataCounter] = num;
        (*dataCounter)++;
    }
    
    return firstStageErr_no_err;
}

static enum firstStageErr fetchData(char *lblName, char *token, int *dataCounter, Symbol **symbols, int **data) {
    enum firstStageErr err;
    Symbol *newS;
    int prevDC;

    prevDC = *dataCounter;
    if ((err = storeDataArgs(getNextToken(token), dataCounter, *symbols, data)) != firstStageErr_no_err)
        return err;
    
    if (*lblName == '\0')
        return firstStageErr_no_err;
    
    newS = (Symbol *)malloc(sizeof(Symbol));
    if (newS == NULL)
        terminalError(1, "Insufficient memory\n");
    
    newS->name = strdup(lblName);
    if (newS->name == NULL)
        terminalError(1, "Insufficient memory\n");
    
    newS->value = prevDC;
    newS->mdefine = 0;

    addSymToList(symbols, newS);
    return firstStageErr_no_err;
}

/**
 * Fetch a number from the start pointer to the given end, result is saved into num.
 * Uses parsing, if the string is not a number tries to find a constant with the same name.
 * @param start The start of the string
 * @param end The end of the string
 * @param num A pointer to the number
 * @param symbols The list of symbols in the program
 * @return The error (if there wasn't an error returns firstStageErr_no_err)
 */
static enum firstStageErr fetchNumber(char *start, char *end, int *num, Symbol *symbols) {
    char temp;
    Symbol *tempSym;
    
    /* some methods used here need '\0' string termination */
    temp = *end;
    *end = '\0';
    
    if (tryParseToken(start, num)) {
        *end = temp;
        return firstStageErr_no_err;
    }

    if (!validSymbolName(start, end - 1)) {
        *end = temp;
        return firstStageErr_data_nan;
    }
    
    if (!getSymbolByName(start, symbols, &tempSym) || !tempSym->mdefine) {
        *end = temp;
        return firstStageErr_data_const_not_found;
    }
    
    logInfo("USING DATA FROM '%s' = %d\n", start, tempSym->value);
    *num = tempSym->value;

    *end = temp;
    return firstStageErr_no_err;
}

static void addSymToList(Symbol **head, Symbol *symbol) {
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
