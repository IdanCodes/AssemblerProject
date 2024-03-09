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
static Symbol *allocSymbol(char *nameStart, char *nameEnd);
static void registerConstant(Symbol **head, char *name, int value);
static void registerDataSymbol(Symbol **head, char *lblName, int value);
static enum firstStageErr fetchConstant(char *line, Symbol **constants);
static enum firstStageErr storeDataArgs(char *token, int *dataCounter, Symbol *symbols, int **data);
static enum firstStageErr fetchData(char *lblName, char *token, int *dataCounter, Symbol **symbols, int **data);
static void storeStringInData(char *quoteStart, char *quoteEnd, int *dataCounter, int **data);
static enum firstStageErr fetchString(char *lblName, char *token, int *dataCounter, Symbol **symbols, int **data);
static enum firstStageErr fetchExtern(char *lblName, char *token, Symbol *symbols);
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
    int len, i, *data;
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
    labelName = NULL;
    symbols = NULL;
    while ((skippedLines = getNextLine(sourcef, line, MAXLINE, &len)) != getLine_FILE_END) {
        sourceLine += skippedLines;
        if (labelName != NULL) {    /* free previous label */
            free(labelName);
            labelName = NULL;
        }
        
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
            
            token = getNextToken(token);
            
            /* empty line */
            if (*token == '\0') {
                printFirstStageError(firstStageErr_label_empty_line, sourceLine);
                continue;
            }
        }

        /* constant declaration? */
        if (tokcmp(token, KEYWORD_CONST_DEC) == 0) {
            if (labelName != NULL)
                printFirstStageError(firstStageErr_const_defined_in_label, sourceLine);
            else if ((err = fetchConstant(line, &symbols)) != firstStageErr_no_err)
                printFirstStageError(err, sourceLine);
            continue;
        }
        
        /* storage instruction? */
        if (tokcmp(token, KEYWORD_DATA_DEC) == 0) {
            if ((err = fetchData(labelName, token, &dataCounter, &symbols, &data)) != firstStageErr_no_err)
                printFirstStageError(err, sourceLine);
            else {
                if (labelName != NULL)
                    logInfo("Added data '%s' - new data counter %d\n", labelName, dataCounter);
                else
                    logInfo("Added unnamed data - new data counter %d\n", dataCounter);
            }
            continue;
        }
        
        if (tokcmp(token, KEYWORD_STRING_DEC) == 0) {
            if ((err = fetchString(labelName, token, &dataCounter, &symbols, &data)) != firstStageErr_no_err)
                printFirstStageError(err, sourceLine);
            else {
                if (labelName != NULL)
                    logInfo("Added string '%s' - new data counter %d (line %u)\n", labelName, dataCounter, sourceLine);
                else
                    logInfo("Added unnamed string - new data counter %d (line %u)\n", dataCounter, sourceLine);
            }
            continue;
        }
        
        /* extern instruction? */
        if (tokcmp(token, KEYWORD_EXTERN_DEC) == 0) {
            if ((err = fetchExtern(labelName, token, symbols)) != firstStageErr_no_err)
                printFirstStageError(err, sourceLine);
            continue;
        }
    }

    
    /* -- cleanup -- */
    
    if (labelName != NULL) {    /* free previous label */
        free(labelName);
        labelName = NULL;
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

/* nameEnd is the first character outside the name (name string is [name, end-1]) */
static Symbol *allocSymbol(char *nameStart, char *nameEnd) {
    Symbol *newS;
    char temp;

    newS = (Symbol *)malloc(sizeof(Symbol));
    if (newS == NULL)
        terminalError(1, "Insufficient memory\n");

    temp = *nameEnd;
    *nameEnd = '\0';

    newS->name = strdup(nameStart);
    if (newS->name == NULL)
        terminalError(1, "Insufficient memory\n");

    *nameEnd = temp;
    return newS;
}

static void registerConstant(Symbol **head, char *name, int value) {
    Symbol *sym;

    sym = allocSymbol(name, getTokEnd(name) + 1);
    sym->value = value;
    sym->flag = SYMBOL_FLAG_MDEFINE;
    
    addSymToList(head, sym);
}

static void registerDataSymbol(Symbol **head, char *lblName, int value) {
    Symbol *newS;
    
    if (lblName == NULL)
        return;
    
    newS = allocSymbol(lblName, getTokEnd(lblName) + 1);
    newS->value = value;
    newS->flag = 0; /* TODO: CHANGE TO USE DATA FLAG */

    addSymToList(head, newS);
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

/* token is the .data token */
static enum firstStageErr fetchData(char *lblName, char *token, int *dataCounter, Symbol **symbols, int **data) {
    enum firstStageErr err;
    int prevDC;

    prevDC = *dataCounter;
    if ((err = storeDataArgs(getNextToken(token), dataCounter, *symbols, data)) != firstStageErr_no_err)
        return err;

    registerDataSymbol(symbols, lblName, prevDC);
    return firstStageErr_no_err;
}

static void storeStringInData(char *quoteStart, char *quoteEnd, int *dataCounter, int **data) {
    int i, len, prevDC;
    
    prevDC = *dataCounter;
    *dataCounter += (len = (int)(quoteEnd - quoteStart));
    *data = realloc(*data, sizeof(int *) * *dataCounter);
    if (*data == NULL)
        terminalError(1, "Insufficient memory\n");

    for (i = 1; i < len; i++)
        (*data)[prevDC + i - 1] = (int)quoteStart[i];

    (*data)[*dataCounter] = '\0';   /* terminate the string */
}

/* token is the .string token */ 
static enum firstStageErr fetchString(char *lblName, char *token, int *dataCounter, Symbol **symbols, int **data) {
    int prevDC;
    char *quoteStart, *quoteEnd;
    
    quoteStart = getFirstOrEnd(token, '"');
    if (*quoteStart == '\0' || (token = getNextToken(token)) < quoteStart)
        return firstStageErr_string_expected_quotes;

    quoteEnd = getFirstOrEnd(token + 1, '"');
    if (*quoteEnd == '\0')
        return firstStageErr_string_expected_end_quotes;
    
    /* check for more chars in the same token and in the rest of the string */
    if (quoteEnd != getTokEnd(quoteEnd) || *getNextToken(quoteEnd) != '\0')
        return firstStageErr_string_extra_chars;
    
    /* TODO: check if a string can be empty */
    
    prevDC = *dataCounter;
    storeStringInData(quoteStart, quoteEnd, dataCounter, data);
    registerDataSymbol(symbols, lblName, prevDC);
    return firstStageErr_no_err;
}

static enum firstStageErr fetchExtern(char *lblName, char *token, Symbol *symbols) {
    char *tokEnd, temp;
    Symbol *tempSymb;
    
    /* TODO: check if there can be multiple extern labels in one .extern instruction */
    token = getNextToken(token);
    if (!validSymbolName(token, tokEnd = getTokEnd(token)))
        return firstStageErr_extern_invalid_lbl_name;

    if (*getNextToken(token) != '\0')
        return firstStageErr_extern_extra_chars;

    temp = *(tokEnd + 1);
    *(tokEnd + 1) = '\0';

    if (lblName != NULL && tokcmp(lblName, token) == 0) {
        *(tokEnd + 1) = temp;
        return firstStageErr_extern_def_label_same_name;
    }

    if (isSavedKeyword(token)) {
        *(tokEnd + 1) = temp;
        return firstStageErr_extern_saved_keyword;
    }

    if (getSymbolByName(token, symbols, &tempSymb)){
        *(tokEnd + 1) = temp;
        return firstStageErr_extern_label_exists;
    }

    *(tokEnd + 1) = temp;

    tempSymb = allocSymbol(token, tokEnd + 1);
    tempSymb->flag = SYMBOL_FLAG_EXTERN;

    addSymToList(&symbols, tempSymb);
    logInfo("Added extern variable '%s'\n", token);
    return firstStageErr_no_err;
}

/**
 * Fetch a number from the start pointer to the given end, result is saved into num.
 * Uses parsing, if the string is not a number tries to find a constant with the same name.
 * @param start The start of the string
 * @param end The end of the string (first character outside it)
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
    
    if (!getSymbolByName(start, symbols, &tempSym) || (tempSym->flag & SYMBOL_FLAG_MDEFINE) == 0) {
        *end = temp;
        return firstStageErr_data_const_not_found;
    }
    
    logInfo("Using data from '%s' = %d\n", start, tempSym->value);
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

static void freeSymbolsList(Symbol *head) {
    if (head == NULL)
        return;

    freeSymbolsList(head->next);
    free(head->name);
    free(head);
}
