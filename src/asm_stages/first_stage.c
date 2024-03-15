#include <stdlib.h>
#include "first_stage.h"
#include "../utils/inpututils.h"
#include "../utils/fileutils.h"
#include "../utils/strutils.h"
#include "../utils/charutils.h"
#include "../utils/logger.h"
#include "../utils/keywords.h"
#include "../utils/operations.h"

static void printFirstStageError(enum firstStageErr err, unsigned int sourceLine, char *fileName);
static char *getErrMessage(enum firstStageErr err);
static int symbolInList(Symbol *head, char *name);
static int getSymbolByName(char *name, Symbol *head, Symbol **pSymbol);
static Symbol *allocSymbol(char *nameStart, char *nameEnd);
static void registerConstant(Symbol **head, char *name, int value);
static void registerDataSymbol(Symbol **head, Symbol *lblSym, int value);
static enum firstStageErr fetchLabel(char **token, char *tokEnd, Symbol **symbols, Symbol **pLblSymbol);
static enum firstStageErr fetchConstant(char *line, Symbol **symbols);
static enum firstStageErr storeDataArgs(char *token, int *dataCounter, Symbol *symbols, int **data);
static enum firstStageErr fetchData(char *token, int *dataCounter, Symbol **symbols, int **data, Symbol *lblSym);
static void storeStringInData(char *quoteStart, char *quoteEnd, int *dataCounter, int **data);
static enum firstStageErr fetchString(char *token, int *dataCounter, Symbol **symbols, int **data, Symbol *lblSym);
static enum firstStageErr fetchExtern(char *token, Symbol *symbols, Symbol *lblSym);
static enum firstStageErr fetchNumber(char *start, char *end, int *num, Symbol *symbols);
static void addSymToList(Symbol **head, Symbol *symbol);
static void freeSymbolsList(Symbol *head);

/* DOCUMENT */
/* fileName is the file's name without the extension */
void assemblerFirstStage(char fileName[]) {
    /* -- declarations -- */
    unsigned int sourceLine, skippedLines;
    int dataCounter, instructionCounter;
    char sourceFileName[FILENAME_MAX]/*, outFileName[FILENAME_MAX]*/, *token, *tokEnd, *sqrBracksOpen, *indexStart, *indexEnd, *sqrBracksClose, temp;
    char line[MAXLINE + 1]; /* account for '\0' */
    int len, *data, num, operandIndex;
    FILE *sourcef;
    Symbol *symbols, *labelSymbol;
    Operation operation;
    enum firstStageErr err;
    
    
    /* -- open files -- */
    sprintf(sourceFileName, "%s.%s", fileName, PRE_ASSEMBLED_FILE_EXTENSION);
    /* TODO: out file? sprintf(outFileName, "%s.%s", fileName, OBJECT_FILE_EXTENSION); */
    
    openFile(sourceFileName, "r", &sourcef);
    
    
    /* -- main loop -- */
    instructionCounter = 0;
    dataCounter = 0;
    data = (int *)malloc(sizeof(int));
    if (data == NULL)
        terminalError(1, "Insufficient memory\n");
    
    sourceLine = 0;
    labelSymbol = NULL;
    symbols = NULL;
    while ((skippedLines = getNextLine(sourcef, line, MAXLINE, &len)) != getLine_FILE_END) {
        sourceLine += skippedLines;
        /* if the previous symbol wasn't used, free it */
        if (labelSymbol != NULL) {
            if (!symbolInList(symbols, labelSymbol->name)) {
                free(labelSymbol->name);
                free(labelSymbol);
            }
            labelSymbol = NULL;
        }
        
        token = getStart(line);
        
        /* label declaration? */
        /* if this is a valid (label) declaration, there must be a white character seperating the label name and the rest of the line */
        if (*(tokEnd = getTokEnd(token)) == LABEL_END_CHAR) 
        {
            if ((err = fetchLabel(&token, tokEnd, &symbols, &labelSymbol)) != firstStageErr_no_err) {
                printFirstStageError(err, sourceLine, sourceFileName);
                continue;
            }
        }

        /* constant declaration? */
        if (tokcmp(token, KEYWORD_CONST_DEC) == 0) {
            if (labelSymbol != NULL)
                printFirstStageError(firstStageErr_label_const_definition, sourceLine, sourceFileName);
            else if ((err = fetchConstant(line, &symbols)) != firstStageErr_no_err)
                printFirstStageError(err, sourceLine, sourceFileName);
            continue;
        }
        
        /* storage instruction? */
        /* .data */
        if (tokcmp(token, KEYWORD_DATA_DEC) == 0) {
            if ((err = fetchData(token, &dataCounter, &symbols, &data, labelSymbol)) != firstStageErr_no_err)
                printFirstStageError(err, sourceLine, sourceFileName);
            else {
                if (labelSymbol != NULL)
                    logInfo("Added data '%s' - new data counter %d\n", labelSymbol->name, dataCounter);
                else
                    logInfo("Added unnamed data - new data counter %d\n", dataCounter);
            }
            continue;
        }
        
        /* .string */
        if (tokcmp(token, KEYWORD_STRING_DEC) == 0) {
            if ((err = fetchString(token, &dataCounter, &symbols, &data, labelSymbol)) != firstStageErr_no_err)
                printFirstStageError(err, sourceLine, sourceFileName);
            else {
                if (labelSymbol != NULL)
                    logInfo("Added string '%s' - new data counter %d (line %u)\n", labelSymbol->name, dataCounter, sourceLine);
                else
                    logInfo("Added unnamed string - new data counter %d (line %u)\n", dataCounter, sourceLine);
            }
            continue;
        }
        
        /* .extern instruction? */
        if (tokcmp(token, KEYWORD_EXTERN_DEC) == 0) {
            if ((err = fetchExtern(token, symbols, labelSymbol)) != firstStageErr_no_err)
                printFirstStageError(err, sourceLine, sourceFileName);
            continue;
        }
        
        /* is this an invalid operation? */
        if (!getOperationByName(token, &operation)) {
            printFirstStageError(firstStageErr_operation_not_found, sourceLine, sourceFileName);
            continue;
        }

        /* add the symbol to the symbols list */
        if (labelSymbol != NULL) {
            labelSymbol->value = instructionCounter;
            labelSymbol->flag = SYMBOL_FLAG_CODE;
            addSymToList(&symbols, labelSymbol);
        }
        
        /* fetch operands */
        for (operandIndex = 0, token = getNextToken(token); operandIndex < NUM_OPERANDS; operandIndex++) {
            if (!operationHasOperand(operation, operandIndex))
                continue;   /* the operation doesn't accept this operand */
            
            if (*token == '\0') {
                printFirstStageError(firstStageErr_operation_expected_operand, sourceLine, sourceFileName);
                goto nextLoop;
            }
            
            /* TODO: use #define to make '#' a constant in char utils or something */
            /* immediate addressing? */
            tokEnd = getEndOfOperand(token);
            if (*token == '#') {
                token++;
                
                if (fetchNumber(token, tokEnd, &num, symbols) != firstStageErr_no_err) {
                    /* TODO: maybe elaborate on the error with the error returned from fetchNumber */
                    printFirstStageError(firstStageErr_operation_invalid_immediate, sourceLine, sourceFileName);
                    goto nextLoop;
                }

                if (!validAddressingMethod(operation, operandIndex, ADDR_IMMEDIATE)) {
                    printFirstStageError(firstStageErr_operation_invalid_addr_method, sourceLine, sourceFileName);
                    goto nextLoop;
                }
                
                /* TODO: here the operand's addressing is immediate */
                logInfo("%s - operand %d = immediate\n", operation.opName, operandIndex);
                
                goto nextOperand;
            }
            
            /* register addressing? */
            temp = *tokEnd;
            *tokEnd = '\0';
            if (isRegisterName(token, &num)) {
                if (!validAddressingMethod(operation, operandIndex, ADDR_REGISTER)) {
                    printFirstStageError(firstStageErr_operation_invalid_addr_method, sourceLine, sourceFileName);
                    goto nextLoop;
                }
                
                /* TODO: here the operand's addressing is register method */
                logInfo("%s - operand %d = register\n", operation.opName, operandIndex);
                
                *tokEnd = temp;
                goto nextOperand;
            }
            *tokEnd = temp;
            
            /* constant indexing addressing? */
            /* TODO: Make the square brackets constants */
            sqrBracksOpen = getFirstOrEnd(token, '[');
            if (sqrBracksOpen < tokEnd) {  /* '[' character must appear next to the array's name (no spaces) */
                /* TODO: maybe make this a function of its own */
                sqrBracksClose = getFirstOrEnd(sqrBracksOpen, ']');
                if (*sqrBracksClose == '\0') {
                    printFirstStageError(firstStageErr_operation_expected_closing_sqr_bracks, sourceLine, sourceFileName);
                    goto nextLoop;
                }
                tokEnd = getEndOfOperand(sqrBracksClose);   /* there could be a space inside the brackets - update the token end */
                
                indexStart = getStart(sqrBracksOpen + 1);   /* skip the spaces between '[' and start of operand */
                if (indexStart == sqrBracksClose) {
                    printFirstStageError(firstStageErr_operation_expected_index, sourceLine, sourceFileName);
                    goto nextLoop;
                }
                
                /* get the last character of the index */
                for (indexEnd = sqrBracksClose - 1; isspace(*indexEnd); indexEnd--)
                    ;
                
                if (getTokEnd(indexStart) < indexEnd) { /* the index is not a single token */
                    printFirstStageError(firstStageErr_operation_invalid_index, sourceLine, sourceFileName);
                    goto nextLoop;
                }
                
                if (fetchNumber(indexStart, indexEnd + 1, &num, symbols) != firstStageErr_no_err) {
                    /* TODO: maybe elaborate on the error with the error returned from fetchNumber */
                    printFirstStageError(firstStageErr_operation_invalid_index, sourceLine, sourceFileName);
                    goto nextLoop;
                }
                
                if (!validSymbolName(token, sqrBracksOpen)) {
                    printFirstStageError(firstStageErr_operation_invalid_label_name, sourceLine, sourceFileName);
                    goto nextLoop;
                }
                
                if (!validAddressingMethod(operation, operandIndex, ADDR_CONSTANT_INDEX)) {
                    printFirstStageError(firstStageErr_operation_invalid_addr_method, sourceLine, sourceFileName);
                    goto nextLoop;
                }
                
                /* TODO: here the operand's addressing is a constant index method */
                logInfo("%s - operand %d = constant index\n", operation.opName, operandIndex);
                
                goto nextOperand;
            }
            else if (validSymbolName(token, tokEnd)) {
                /* the operand must be a direct (label) addressing */
                if (!validAddressingMethod(operation, operandIndex, ADDR_DIRECT)) {
                    logInfo("Operation %s does not accept direct addressing as operand %d\n", operation.opName, operandIndex);
                    printFirstStageError(firstStageErr_operation_invalid_addr_method, sourceLine, sourceFileName);
                    goto nextLoop;
                }
                
                /* TODO: here the operand's addressing is a direct (label) addressing */
                logInfo("%s - operand %d = direct\n", operation.opName, operandIndex);
                
                goto nextOperand;
            }
            else {
                printFirstStageError(firstStageErr_operation_invalid_operand, sourceLine, sourceFileName);
                goto nextLoop;
            }
            
            nextOperand:
            tokEnd = getStart(tokEnd);  /* skip spaces */
            if (*tokEnd != '\0' && *tokEnd != ',') {
                printFirstStageError(firstStageErr_operation_expected_comma, sourceLine, sourceFileName);
                goto nextLoop;
            }
            
            if (*tokEnd != '\0')
                token = getStart(tokEnd + 1);
            else
                token = tokEnd;
        }
        
        /* check for extra text following the operands */
        if (*token != '\0') {
            printFirstStageError(firstStageErr_operation_extra_chars, sourceLine, sourceFileName);
            goto nextLoop;
        }
        
        nextLoop:
        continue;
    }

    
    /* -- cleanup -- */
    
    if (labelSymbol != NULL && !symbolInList(symbols, labelSymbol->name)) {   /* free previous label */
        free(labelSymbol->name);
        free(labelSymbol);
    }

    free(data);
    freeSymbolsList(symbols);
}

static void printFirstStageError(enum firstStageErr err, unsigned int sourceLine, char *fileName) {
    char *message;
    
    if (err == firstStageErr_no_err)
        return;
    
    message = getErrMessage(err);
    logErr("file \"%s\" line %u - %s\n", fileName, sourceLine, message);
}

static char *getErrMessage(enum firstStageErr err) {
    switch (err) {
        /* -- .define -- */
        case firstStageErr_define_name_expected:
            return "constant name expected";
            
        case firstStageErr_define_unexpected_chars:
            return "unexpected characters at end of line";
            
        case firstStageErr_define_expected_equal_sign:
            return "equal sign expeceted";
            
        case firstStageErr_define_value_expected:
            return "constant value expected";
            
        case firstStageErr_define_invalid_name:
            return "invalid name for a constant";
            
        case firstStageErr_define_saved_keyword:
            return "can't use a saved keyword as a constant's name";
            
        case firstStageErr_define_name_taken:
            return "the constant's name is already used";
            
        case firstStageErr_define_value_nan:
            return "value is not a number";
        
            
        /* -- label -- */
        case firstStageErr_label_invalid_name:
            return "invalid name for a label";
            
        case firstStageErr_label_const_definition:
            return "can't define a constant in the same line as a label";
            
        case firstStageErr_label_saved_keyword:
            return "can't use a saved keyword as a label's name";
            
        case firstStageErr_label_name_taken:
            return "a label with this name already exists";
            
        case firstStageErr_label_empty_line:
            return "can't define a label on an empty line";
            
            
        /* -- .data -- */
        case firstStageErr_data_nan:
            return "arguments of a data instruction must be immediate numbers or constants";
            
        case firstStageErr_data_comma_expected:
            return "comma expected between arguments";
            
        case firstStageErr_data_const_not_found:
            return "constant was not found";
            
        case firstStageErr_data_argument_expected:
            return "argument expected";
            
            
        /* -- .string -- */
        case firstStageErr_string_expected_quotes:
            return "expected '\"' to start the string";
        case firstStageErr_string_expected_end_quotes:
            return "expected '\"' to terminate the string";
        case firstStageErr_string_extra_chars:
            return "extra characters after closing '\"'";
            
            
        /* -- .extern -- */
        case firstStageErr_extern_invalid_lbl_name:
            return "invalid label name for extern label";
            
        case firstStageErr_extern_extra_chars:
            return "extra characters following external label";
            
        case firstStageErr_extern_def_label_same_name:
            ;   /* fall through */
        case firstStageErr_extern_label_exists:
            return "a label with this name already exists in this file";
            
        case firstStageErr_extern_saved_keyword:
            return "can't used .extern on a saved keyword";
            
            
        /* -- operations -- */
        case firstStageErr_operation_not_found:
            return "invalid operation";
            
        case firstStageErr_operation_expected_operand:
            return "operand expected";
            
        case firstStageErr_operation_invalid_immediate:
            return "immediate operand must be a number or constant";
            
        case firstStageErr_operation_expected_closing_sqr_bracks:
            return "expected closing square brackets";
            
        case firstStageErr_operation_expected_index:
            return "index expected inside square brackets";
            
        case firstStageErr_operation_invalid_index:
            return "index must be a number or constant";
            
        case firstStageErr_operation_invalid_label_name:
            return "operand has an invalid label name";
        
        case firstStageErr_operation_invalid_addr_method:
            return "addressing method does not match operation";
        
        case firstStageErr_operation_invalid_operand:
            return "illegal operand";
        
        case firstStageErr_operation_expected_comma:
            return "comma expected to seperate operands";
        
        case firstStageErr_operation_extra_chars:
            return "extra characters at end of line";
            
            
        default:
            return "UNDEFINED ERROR";
    }
}

/**
 * Is the given string a valid name for a symbol?
 * @param start the start of the symbol name
 * @param end the end of the symbol name (first character *outside* the name)
 * @return 1 if the string is a valid name for a symbol, 0 otherwise
 */
int validSymbolName(char *start, char *end) {
    if (end < start || (end - start) >= LABEL_MAX_LENGTH ||  /* invalid length */
        !isalpha(*start))    /* first character is not alphabetic */
        return 0;

    for (start++; start < end; start++) {
        if (!isalnum(*start))
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

static int symbolInList(Symbol *head, char *name) {
    Symbol *tempSym;
    return getSymbolByName(name, head, &tempSym);
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

static void registerDataSymbol(Symbol **head, Symbol *lblSym, int value) {
    if (lblSym == NULL)
        return;
    
    lblSym->value = value;
    lblSym->flag = SYMBOL_FLAG_DATA;

    addSymToList(head, lblSym);
}

static enum firstStageErr fetchLabel(char **token, char *tokEnd, Symbol **symbols, Symbol **pLblSymbol) {
    int i;
    char *labelName;
    
    /* TODO: store the label in the symbols list even if the rest of the line throws an error */
    if (!validSymbolName(*token, tokEnd - 1))
        return firstStageErr_label_invalid_name;

    /* save label name */
    labelName = (char *)malloc(sizeof(char [LABEL_MAX_LENGTH]));
    if (labelName == NULL)
        terminalError(1, "Insufficient memory\n");

    for (i = 0; i < tokEnd - (*token); i++)
        (labelName)[i] = (*token)[i];
    (labelName)[i] = '\0';

    *pLblSymbol = allocSymbol(labelName, getTokEnd(labelName) + 1);

    if (isSavedKeyword(labelName))
        return firstStageErr_label_saved_keyword;

    if (symbolInList(*symbols, labelName))
        return firstStageErr_label_name_taken;

    *token = getNextToken(*token);

    /* empty line */
    if (**token == '\0')
        return firstStageErr_label_empty_line;
    
    return firstStageErr_no_err;
}

static enum firstStageErr fetchConstant(char *line, Symbol **symbols) {
    char *name, *token, *equalsSign, *strVal, tempC;
    int numberValue;
    
    token = getStart(line);
    
    /* fetch name + syntax validation */
    name = (token = getNextToken(token));
    equalsSign = getFirstOrEnd(token, '=');
    
    if (name == equalsSign) /* equals sign appeared before name */
        return firstStageErr_define_name_expected;
    
    if (*equalsSign == '\0' || getNextToken(name) < equalsSign)    /* equal sign expected */
        return firstStageErr_define_expected_equal_sign;

    /* fetch strVal + syntax validation */
    strVal = (token = getNextToken(equalsSign));
    if (*strVal == '\0')
        return firstStageErr_define_value_expected;
    
    if (*getNextToken(token) != '\0')   /* extra characters */
        return firstStageErr_define_unexpected_chars;

    /* make sure the constant's name is valid */
    if (!validSymbolName(name, getTokEnd(name)))
        return firstStageErr_define_invalid_name;

    /* check if the name is a saved keyword */
    if (isSavedKeyword(name))
        return firstStageErr_define_saved_keyword;
    
    /* check if the name is an existing symbols' names */
    if (*symbols != NULL && symbolInList(*symbols, name))
        return firstStageErr_define_name_taken;

    /* make sure the numberValue is a number */
    if (!tryParseToken(strVal, &numberValue))
        return firstStageErr_define_value_nan;

    /* register the new constant */
    registerConstant(symbols, name, numberValue);
    
    /* TODO: remove this debug log */
    tempC = *(getTokEnd(name) + 1) = '\0';
    logInfo("Registered constant '%s': %d.\n", name, numberValue);
    *(getTokEnd(name) + 1) = tempC;
    
    return firstStageErr_no_err;
}

/* token is the first number parameter */
static enum firstStageErr storeDataArgs(char *token, int *dataCounter, Symbol *symbols, int **data) {
    int num;
    char *end, *next, *valEnd;
    enum firstStageErr err;
    
    token = getStart(token);
    if (*token == '\0')
        return firstStageErr_data_argument_expected;
    
    while (*token != '\0') {
        if (*token == ',')
            return firstStageErr_data_argument_expected;
        
        end = getFirstOrEnd(token, ',');
        next = (*end != '\0') ? getStart(end + 1) : getNextToken(token);
        
        if ((*end != '\0' && end > next) || (*end == '\0' && *next != '\0'))
            return firstStageErr_data_comma_expected;
        
        if (*next == ',' || (*end != '\0' && *next == '\0'))
            return firstStageErr_data_argument_expected;
        
        valEnd = getTokEnd(token) + 1;
        valEnd = valEnd < end ? valEnd : end;   /* whatever comes first: the ',' char or the end of the token */
        if ((err = fetchNumber(token, valEnd, &num, symbols)) != firstStageErr_no_err)
            return err;
        
        token = next;
        
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
static enum firstStageErr fetchData(char *token, int *dataCounter, Symbol **symbols, int **data, Symbol *lblSym) {
    enum firstStageErr err;
    int prevDC;

    prevDC = *dataCounter;
    if ((err = storeDataArgs(getNextToken(token), dataCounter, *symbols, data)) != firstStageErr_no_err)
        return err;

    registerDataSymbol(symbols, lblSym, prevDC);
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
static enum firstStageErr fetchString(char *token, int *dataCounter, Symbol **symbols, int **data, Symbol *lblSym) {
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
    registerDataSymbol(symbols, lblSym, prevDC);
    return firstStageErr_no_err;
}

static enum firstStageErr fetchExtern(char *token, Symbol *symbols, Symbol *lblSym) {
    char *tokEnd, temp;
    Symbol *tempSym;
    
    token = getNextToken(token);
    if (!validSymbolName(token, tokEnd = getTokEnd(token)))
        return firstStageErr_extern_invalid_lbl_name;

    if (*getNextToken(token) != '\0')
        return firstStageErr_extern_extra_chars;

    temp = *(tokEnd + 1);
    *(tokEnd + 1) = '\0';

    if (lblSym != NULL && tokcmp(lblSym->name, token) == 0) {
        *(tokEnd + 1) = temp;
        return firstStageErr_extern_def_label_same_name;
    }

    if (isSavedKeyword(token)) {
        *(tokEnd + 1) = temp;
        return firstStageErr_extern_saved_keyword;
    }

    if (getSymbolByName(token, symbols, &tempSym) && tempSym->flag != SYMBOL_FLAG_EXTERN) {
        *(tokEnd + 1) = temp;
        return firstStageErr_extern_label_exists;
    }

    *(tokEnd + 1) = temp;

    tempSym = allocSymbol(token, tokEnd + 1);
    tempSym->flag = SYMBOL_FLAG_EXTERN;

    addSymToList(&symbols, tempSym);
    
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
