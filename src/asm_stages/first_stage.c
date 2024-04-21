#include <stdlib.h>
#include "first_stage.h"
#include "../utils/inpututils.h"
#include "../utils/fileutils.h"
#include "../utils/strutils.h"
#include "../utils/charutils.h"
#include "../utils/logger.h"
#include "../utils/keywords.h"

static void printFirstStageError(enum firstStageErr err, unsigned int sourceLine, char *fileName);
static char *getErrMessage(enum firstStageErr err);
static void registerConstant(Symbol **head, char *name, int value);
static void registerDataSymbol(Symbol **head, Symbol *lblSym, int value, int length);
static enum firstStageErr fetchLabel(char **token, char *tokEnd, Symbol **pLblSymbol);
static enum firstStageErr fetchConstant(char *line, Symbol **symbols, Macro *macros);
static enum firstStageErr storeDataArgs(char *token, int *dataCounter, Symbol *symbols, int **data);
static enum firstStageErr fetchData(char *token, int *dataCounter, Symbol **symbols, int **data, Symbol *lblSym, Macro *macros);
static void storeStringInData(char *quoteStart, char *quoteEnd, int *dataCounter, int **data);
static enum firstStageErr fetchString(char *token, int *dataCounter, Symbol **symbols, int **data, Symbol *lblSym, Macro *macros);
static enum firstStageErr fetchExtern(char *token, Symbol **symbols, Symbol *lblSym, Macro *macros);
static enum firstStageErr validateEntry(char *token, Symbol *lblSym);
static enum firstStageErr fetchNumber(char *start, char *end, int *num, Symbol *symbols);
static enum firstStageErr fetchOperands(char *token, Operation operation, Symbol *symbols, Macro *macros, Byte words[NUM_MAX_EXTRA_WORDS], int *wordIndex, char operandAddrs[NUM_OPERANDS]);

/* DOCUMENT */
/* fileName is the file's name without the extension */
/* returns 1 if there was no error, 0 if there was one */
int assemblerFirstStage(char fileName[], int **data, Macro *macros, Symbol **symbols, ByteNode **bytes, int *instructionCounter, int *dataCounter) {
    /* -- declarations -- */
    unsigned int sourceLine, skippedLines;
    char sourceFileName[FILENAME_MAX], *token, *tokEnd, operandAddrs[NUM_OPERANDS];
    char line[MAXLINE + 1]; /* account for '\0' */
    int numWords, i, hasErr;
    FILE *sourcef;
    Symbol *labelSymbol, *curr;
    Operation operation;
    Byte firstWord, words[NUM_MAX_EXTRA_WORDS], tempByte;
    enum firstStageErr err;
    
    
    /* -- open file -- */
    sprintf(sourceFileName, "%s.%s", fileName, PRE_ASSEMBLED_FILE_EXTENSION);
    openFile(sourceFileName, "r", &sourcef);
    
    
    /* -- main loop -- */
    *data = (int *)malloc(sizeof(int));
    if (*data == NULL)
        logInsuffMemErr("allocating data for first stage");
    
    *instructionCounter = 0;
    *dataCounter = 0;
    sourceLine = 0;
    labelSymbol = NULL;
    *symbols = NULL;
    hasErr = 0;
    *bytes = NULL;
    while (getNextLine(sourcef, line, 1, MAXLINE, &skippedLines) != getLine_FILE_END) {
        sourceLine += skippedLines;
        /* if the previous symbol wasn't used, free it */
        if (labelSymbol != NULL) {
            if (!symbolInList(*symbols, labelSymbol->name)) {
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
            if ((err = fetchLabel(&token, tokEnd, &labelSymbol)) != firstStageErr_no_err) {
                printFirstStageError(err, sourceLine, sourceFileName);
                hasErr = 1;
                continue;
            }
        }

        /* constant declaration? */
        if (tokcmp(token, KEYWORD_CONST_DEC) == 0) {
            if (labelSymbol != NULL) {
                printFirstStageError(firstStageErr_label_const_definition, sourceLine, sourceFileName);
                hasErr = 1;
            }
            else if ((err = fetchConstant(line, symbols, macros)) != firstStageErr_no_err) {
                printFirstStageError(err, sourceLine, sourceFileName);
                hasErr = 1;
            }
            continue;
        }
        
        /* storage instruction? */
        /* .data */
        if (tokcmp(token, KEYWORD_DATA_DEC) == 0) {
            if ((err = fetchData(token, dataCounter, symbols, data, labelSymbol, macros)) != firstStageErr_no_err) {
                printFirstStageError(err, sourceLine, sourceFileName);
                hasErr = 1;
            }
            continue;
        }
        
        /* .string */
        if (tokcmp(token, KEYWORD_STRING_DEC) == 0) {
            if ((err = fetchString(token, dataCounter, symbols, data, labelSymbol, macros)) != firstStageErr_no_err) {
                printFirstStageError(err, sourceLine, sourceFileName);
                hasErr = 1;
            }
            continue;
        }
        
        /* .extern instruction? */
        if (tokcmp(token, KEYWORD_EXTERN_DEC) == 0) {
            if ((err = fetchExtern(token, symbols, labelSymbol, macros)) != firstStageErr_no_err) {
                if (err == firstStageErr_extern_exists)
                    logWarn("label was already declared as extern (in file \"%s\", line %u)\n", sourceFileName, sourceLine);
                else if (err == firstStageErr_extern_define_label)
                    logWarn("ignoring label defined in .extern instruction (in file \"%s\", line %u)\n", sourceFileName, sourceLine);
                else {
                    printFirstStageError(err, sourceLine, sourceFileName);
                    hasErr = 1;
                }
            }
            continue;
        }
        
        /* .entry instruction? */
        if (tokcmp(token, KEYWORD_ENTRY_DEC) == 0) {
            if ((err = validateEntry(token, labelSymbol)) != firstStageErr_no_err) {
                if (err == firstStageErr_entry_define_label)
                    logWarn("ignoring label defined in .entry instruction (in file \"%s\", line %u)\n", sourceFileName, sourceLine);
                else {
                    printFirstStageError(err, sourceLine, sourceFileName);
                    hasErr = 1;
                }
            }
            continue;
        }
        
        /* is this an invalid operation? */
        if (!getOperationByName(token, &operation)) {
            printFirstStageError(firstStageErr_operation_not_found, sourceLine, sourceFileName);
            hasErr = 1;
            continue;
        }

        /* add the symbol to the symbols list */
        if (labelSymbol != NULL) {
            if (symbolInList(*symbols, labelSymbol->name)) {
                printFirstStageError(firstStageErr_label_name_taken, sourceLine, sourceFileName);
                hasErr = 1;
                continue;
            }
            
            if (getMacroWithName(labelSymbol->name, macros) != NULL) {
                printFirstStageError(firstStageErr_label_macro_name, sourceLine, sourceFileName);
                hasErr = 1;
                continue;
            }
            
            labelSymbol->value = *instructionCounter + INSTRUCTION_COUNTER_OFFSET;
            labelSymbol->flags = SYMBOL_FLAG_CODE;
            addSymToList(symbols, labelSymbol);
        }
        
        /* fetch operands */
        numWords = 0;
        if ((err = fetchOperands(token, operation, *symbols, macros, words, &numWords, operandAddrs)) != firstStageErr_no_err) {
            printFirstStageError(err, sourceLine, sourceFileName);
            hasErr = 1;
            
            continue;
        }
        
        getFirstWordBin(operation.opCode, operandAddrs[SOURCE_OPERAND_INDEX], operandAddrs[DEST_OPERAND_INDEX], &firstWord);
        
        /* "merge" the operands if both are register addressing */
        if (operandAddrs[SOURCE_OPERAND_INDEX] == ADDR_REGISTER && operandAddrs[DEST_OPERAND_INDEX] == ADDR_REGISTER) {
            bytesOrGate(words[SOURCE_OPERAND_INDEX], words[DEST_OPERAND_INDEX], &words[SOURCE_OPERAND_INDEX]);
            numWords--;    /* only has one extra word */
        }
        *instructionCounter += numWords + 1;
        
        /* write instructions to file */
        addByteNodeToList(bytes, copyByte(firstWord));
        for (i = 0; i < numWords; i++)
            addByteNodeToList(bytes, copyByte(words[i]));
    }
    
    /* add IC+100 to the .data symbols' values */
    for (curr = *symbols; curr != NULL; curr = curr->next) {
        if ((curr->flags & SYMBOL_FLAG_DATA) == 0)
            continue;

        curr->value += *instructionCounter + INSTRUCTION_COUNTER_OFFSET;
    }
    
    /* write data to file */
    for (i = 0; i < *dataCounter; i++) {
        numberToByte((*data)[i], &tempByte);
        addByteNodeToList(bytes, copyByte(tempByte));
    }

    
    /* -- cleanup -- */
    
    if (labelSymbol != NULL && !symbolInList(*symbols, labelSymbol->name)) {   /* free previous label */
        free(labelSymbol->name);
        free(labelSymbol);
    }
    
    return hasErr;
}

/**
 * Prints a firstStageErr error message in association with the line in the file it occurred and the file's name.
 * @param err The error to print
 * @param sourceLine The line the error occured on in the file
 * @param fileName The name of the file the error occured on
 */
static void printFirstStageError(enum firstStageErr err, unsigned int sourceLine, char *fileName) {
    char *message;
    
    if (err == firstStageErr_no_err)
        return;
    
    message = getErrMessage(err);
    logErr("file \"%s\" line %u - %s\n", fileName, sourceLine, message);
}

/**
 * Returns string error message of a firstStageErr
 * @param err The error
 * @return The desired error message
 */
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
            return "constant value must be an immediate whole number";
            
        case firstStageErr_define_macro_name:
            return "can't define a constant with the same name as a macro";
        
            
        /* -- label -- */
        case firstStageErr_label_invalid_name:
            return "invalid name for a label";
            
        case firstStageErr_label_const_definition:
            return "can't define a constant in the same line as a label";
            
        case firstStageErr_label_saved_keyword:
            return "can't use a saved keyword as a label's name";
            
        case firstStageErr_label_name_taken:
            return "a symbol with this name already exists";
            
        case firstStageErr_label_empty_line:
            return "can't define a label on an empty line";
            
        case firstStageErr_label_macro_name:
            return "can't define a label with the same name as a macro";
            
        /* -- .data -- */
        case firstStageErr_data_nan:
            return "arguments of a data instruction must be immediate numbers or constants";
            
        case firstStageErr_data_comma_expected:
            return "comma expected between arguments";
            
        case firstStageErr_data_const_not_found:
            return "constant was not found";
            
        case firstStageErr_data_argument_expected:
            return "argument expected";
            
        case firstStageErr_data_oor:
            return "data argument out of range for a byte";
            
            
        /* -- .string -- */
        case firstStageErr_string_expected_quotes:
            return "expected '\"' to start the string";
            
        case firstStageErr_string_expected_end_quotes:
            return "expected '\"' to terminate the string";
            
        case firstStageErr_string_extra_chars:
            return "extra characters after closing '\"'";
            
        case firstStageErr_string_not_printable:
            return "string is not printable";
            
            
        /* -- .extern -- */
        case firstStageErr_extern_invalid_lbl_name:
            return "invalid label name for extern argument";
            
        case firstStageErr_extern_extra_chars:
            return "extra characters following .extern argument";
            
        case firstStageErr_extern_label_exists:
            return "a symbol with this name already exists in this file";
            
        case firstStageErr_extern_saved_keyword:
            return "can't use .extern on a saved keyword";
            
        case firstStageErr_extern_macro_name:
            return ".extern argument has the same name as a macro";
          
            
        /* -- .entry -- */
        case firstStageErr_entry_invalid_lbl_name:
            return "invalid label name for entry argument";
            
        case firstStageErr_entry_extra_chars:
            return "extra characters following .entry argument";
            
            
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
            return "addressing methods don't match operation";
        
        case firstStageErr_operation_invalid_operand:
            return "illegal operand";
        
        case firstStageErr_operation_expected_comma:
            return "comma expected to seperate operands";
            
        case firstStageErr_operation_immediate_oor:
            return "immediate operand is out of range";
            
        case firstStageErr_operation_index_oor:
            return "index is out of range";
            
        case firstStageErr_operation_too_many_operands:
            return "too many operands";
            
        case firstStageErr_operation_extra_chars:
            return "extra characters at the end of the line";
            
        case firstStageErr_operation_operand_number:
            return "a number operand has to follow a #";
            
        case firstStageErr_operation_operand_macro:
            return "macro can't be used as an operand";
            
        default:
            return "UNDEFINED ERROR";
    }
}

static void registerConstant(Symbol **head, char *name, int value) {
    Symbol *sym;

    sym = allocSymbol(name, getTokEnd(name) + 1);
    sym->value = value;
    sym->flags = SYMBOL_FLAG_MDEFINE;
    
    addSymToList(head, sym);
}

static void registerDataSymbol(Symbol **head, Symbol *lblSym, int value, int length) {
    /* lblSym is not NULL here */
    lblSym->value = value;
    lblSym->length = length;
    lblSym->flags = SYMBOL_FLAG_DATA;

    addSymToList(head, lblSym);
}

static enum firstStageErr fetchLabel(char **token, char *tokEnd, Symbol **pLblSymbol) {
    int i;
    char *labelName;
    
    if (!validSymbolName(*token, tokEnd - 1))
        return firstStageErr_label_invalid_name;

    /* save label name */
    labelName = (char *)malloc(sizeof(char) * (tokEnd - (*token)));
    if (labelName == NULL)
        logInsuffMemErr("allocating label");

    for (i = 0; i < tokEnd - (*token); i++)
        (labelName)[i] = (*token)[i];
    (labelName)[i] = '\0';
    
    *pLblSymbol = allocSymbol(labelName, getTokEnd(labelName) + 1);

    if (isSavedKeyword(labelName))
        return firstStageErr_label_saved_keyword;

    *token = getNextToken(*token);

    /* empty line */
    if (**token == '\0')
        return firstStageErr_label_empty_line;
    
    return firstStageErr_no_err;
}

static enum firstStageErr fetchConstant(char *line, Symbol **symbols, Macro *macros) {
    char *name, *token, *equalsSign, *strVal, *nameEnd;
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
    strVal = (token = getStart(equalsSign + 1));
    if (*strVal == '\0')
        return firstStageErr_define_value_expected;
    
    if (*getNextToken(token) != '\0')   /* extra characters */
        return firstStageErr_define_unexpected_chars;

    /* make sure the constant's name is valid */
    /* check if the equals sign is in the same token as the name -
     * - if it is, the end of the name is (equalsSign - 1), else it's the end of the name token */
    nameEnd = isInTok(name, equalsSign) ? equalsSign - 1 : getTokEnd(name);
    if (!validSymbolName(name, nameEnd))
        return firstStageErr_define_invalid_name;

    /* check if the name is a saved keyword */
    *equalsSign = '\0'; /* so the constant's name will be read correctly */
    if (isSavedKeyword(name)) {
        *equalsSign = '=';
        return firstStageErr_define_saved_keyword;
    }
    
    /* check if the name is an existing symbols' names */
    if (*symbols != NULL && symbolInList(*symbols, name)) {
        *equalsSign = '=';
        return firstStageErr_define_name_taken;
    }
    
    /* check if the name is a macro's name */
    if (getMacroWithName(name, macros) != NULL) {
        *equalsSign = '=';
        return firstStageErr_define_macro_name;
    }

    /* make sure the numberValue is a number */
    if (!tryParseToken(strVal, &numberValue)) {
        *equalsSign = '=';
        return firstStageErr_define_value_nan;
    }

    /* register the new constant */
    registerConstant(symbols, name, numberValue);
    *equalsSign = '=';
    
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
        
        if ((*end != '\0' && getStart(getTokEnd(token) + 1) < end) || (*end == '\0' && *next != '\0'))
            return firstStageErr_data_comma_expected;
        
        if (*next == ',' || (*end != '\0' && *next == '\0'))
            return firstStageErr_data_argument_expected;
        
        valEnd = getTokEnd(token) + 1;
        valEnd = valEnd < end ? valEnd : end;   /* whatever comes first: the ',' char or the end of the token */
        if ((err = fetchNumber(token, valEnd, &num, symbols)) != firstStageErr_no_err)
            return err;
        
        if (!inByteRange(num))
            return firstStageErr_data_oor;
        
        token = next;
        
        /* store the number into memory and increment data counter */
        *data = (int *)realloc(*data, sizeof(int) * (*dataCounter + 1));
        if (*data == NULL)
            logInsuffMemErr("reallocating data for numbers in first stage");
        
        (*data)[*dataCounter] = num;
        (*dataCounter)++;
    }
    
    return firstStageErr_no_err;
}

/* token is the .data token */
static enum firstStageErr fetchData(char *token, int *dataCounter, Symbol **symbols, int **data, Symbol *lblSym, Macro *macros) {
    enum firstStageErr err;
    int prevDC;

    prevDC = *dataCounter;
    if ((err = storeDataArgs(getNextToken(token), dataCounter, *symbols, data)) != firstStageErr_no_err)
        return err;
    
    if (lblSym == NULL)
        return firstStageErr_no_err;
    
    if (symbolInList(*symbols, lblSym->name))
        return firstStageErr_label_name_taken;
    
    if (getMacroWithName(lblSym->name, macros) != NULL)
        return firstStageErr_label_macro_name;
    
    registerDataSymbol(symbols, lblSym, prevDC, *dataCounter - prevDC);
    return firstStageErr_no_err;
}

static void storeStringInData(char *quoteStart, char *quoteEnd, int *dataCounter, int **data) {
    int i, len, prevDC;
    
    prevDC = *dataCounter;
    *dataCounter += (len = (int)(quoteEnd - quoteStart));
    *data = realloc(*data, sizeof(int *) * *dataCounter);
    if (*data == NULL)
        logInsuffMemErr("reallocating data for string in first stage");

    for (i = 1; i < len; i++)
        (*data)[prevDC + i - 1] = (int)quoteStart[i];

    (*data)[*dataCounter] = '\0';   /* terminate the string */
}

/* token is the .string token */ 
static enum firstStageErr fetchString(char *token, int *dataCounter, Symbol **symbols, int **data, Symbol *lblSym, Macro *macros) {
    int prevDC;
    char *quoteStart, *quoteEnd, *pc;
    
    quoteStart = getFirstOrEnd(token, '"');
    if (*quoteStart == '\0' || getNextToken(token) < quoteStart)
        return firstStageErr_string_expected_quotes;

    for (quoteEnd = quoteStart + 1; *quoteEnd != '\0'; quoteEnd++)
        ;   /* find the end of the string */
    for (; *quoteEnd != '"' && quoteEnd > quoteStart; quoteEnd--)
        ;   /* find the last '"' of the string */
        
    if (quoteEnd == quoteStart)
        return firstStageErr_string_expected_end_quotes;
    
    /* check for more chars in the same token and in the rest of the string */
    if (quoteEnd != getTokEnd(quoteEnd) || *getNextToken(quoteEnd) != '\0')
        return firstStageErr_string_extra_chars;

    if (lblSym != NULL) {
        if (symbolInList(*symbols, lblSym->name))
            return firstStageErr_label_name_taken;
        if (getMacroWithName(lblSym->name, macros))
            return firstStageErr_label_macro_name;
    }
    
    /* check if the string is printable */
    for (pc = quoteStart + 1; pc < quoteEnd; pc++) {
        if (!isprint(*pc))
            return firstStageErr_string_not_printable;
    }
    
    prevDC = *dataCounter;
    storeStringInData(quoteStart, quoteEnd, dataCounter, data);
    
    if (lblSym != NULL)
        registerDataSymbol(symbols, lblSym, prevDC, *dataCounter - prevDC);
    
    
    return firstStageErr_no_err;
}

static enum firstStageErr fetchExtern(char *token, Symbol **symbols, Symbol *lblSym, Macro *macros) {
    char *tokEnd, temp;
    Symbol *tempSym;
    enum firstStageErr err;
    
    err = firstStageErr_no_err;
    
    token = getNextToken(token);
    if (!validSymbolName(token, tokEnd = getTokEnd(token)))
        return firstStageErr_extern_invalid_lbl_name;

    if (*getNextToken(token) != '\0')
        return firstStageErr_extern_extra_chars;
    
    if (lblSym != NULL)
        err = firstStageErr_extern_define_label;
    
    temp = *(tokEnd + 1);
    *(tokEnd + 1) = '\0';
    
    if (isSavedKeyword(token)) {
        *(tokEnd + 1) = temp;
        return firstStageErr_extern_saved_keyword;
    }
    
    if (getSymbolByName(token, *symbols, &tempSym)) {
        if ((tempSym->flags & SYMBOL_FLAG_EXTERN) == 0) {
            *(tokEnd + 1) = temp;
            return firstStageErr_extern_label_exists;
        }
        else
            err = firstStageErr_extern_exists;
    }
    
    if (getMacroWithName(token, macros) != NULL) {
        *(tokEnd + 1) = temp;
        return firstStageErr_extern_macro_name;
    }

    *(tokEnd + 1) = temp;

    tempSym = allocSymbol(token, tokEnd + 1);
    tempSym->flags = SYMBOL_FLAG_EXTERN;

    addSymToList(symbols, tempSym);
    return err;
}

static enum firstStageErr validateEntry(char *token, Symbol *lblSym) {
    enum firstStageErr err;
    err = firstStageErr_no_err;
    
    token = getNextToken(token);
    if (!validSymbolName(token, getTokEnd(token)))
        return firstStageErr_entry_invalid_lbl_name;

    if (*getNextToken(token) != '\0')
        return firstStageErr_entry_extra_chars;
    
    if (lblSym != NULL)
        err = firstStageErr_entry_define_label;
    
    return err;
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
    
    if (!getSymbolByName(start, symbols, &tempSym) || (tempSym->flags & SYMBOL_FLAG_MDEFINE) == 0) {
        *end = temp;
        return firstStageErr_data_const_not_found;
    }
    
    *num = tempSym->value;
    *end = temp;
    return firstStageErr_no_err;
}

static enum firstStageErr fetchOperands(char *token, Operation operation, Symbol *symbols, Macro *macros, Byte words[NUM_MAX_EXTRA_WORDS], int *wordIndex, char operandAddrs[NUM_OPERANDS]) {
    int operandIndex, num, numOps;
    char temp, *tokEnd, *sqrBracksOpen, *sqrBracksClose, *indexStart, *indexEnd;
    
    *wordIndex = 0;
    numOps = getOperandCount(operation);
    for (operandIndex = 0, token = getNextToken(token); *token != '\0'; operandIndex++) {
        if (operandIndex > numOps)
            return firstStageErr_operation_too_many_operands;
        
        operandAddrs[operandIndex] = 0;
        if (!operationHasOperand(operation, operandIndex))
            continue;   /* the operation doesn't accept this operand */

        if (*token == '\0')
            return firstStageErr_operation_expected_operand;

        /* immediate addressing? */
        tokEnd = getEndOfOperand(token);
        if (*token == IMMEDIATE_OPERAND_PREFIX_CHAR) {
            token++;
            
            if (fetchNumber(token, tokEnd, &num, symbols) != firstStageErr_no_err)
                return firstStageErr_operation_invalid_immediate;

            if (!validAddressingMethod(operation, operandIndex, ADDR_IMMEDIATE))
                return firstStageErr_operation_invalid_addr_method;
            
            operandAddrs[operandIndex] = ADDR_IMMEDIATE;

            if (!writeImmediateToByte(&words[(*wordIndex)++], num))
                return firstStageErr_operation_immediate_oor;
            goto nextOperand;
        }

        /* register addressing? */
        temp = *tokEnd;
        *tokEnd = '\0';
        if (isRegisterName(token, &num)) {
            if (!validAddressingMethod(operation, operandIndex, ADDR_REGISTER)) {
                *tokEnd = temp;
                return firstStageErr_operation_invalid_addr_method;
            }
            
            operandAddrs[operandIndex] = ADDR_REGISTER;
            writeRegisterToByte(&words[(*wordIndex)++], num, operandIndex);
            
            *tokEnd = temp;
            goto nextOperand;
        }
        *tokEnd = temp;

        /* constant indexing addressing? */
        sqrBracksOpen = getFirstOrEnd(token, OPERAND_INDEX_START_CHAR);
        if (sqrBracksOpen < tokEnd) {  /* '[' character must appear next to the array's name (no spaces) */
            sqrBracksClose = getFirstOrEnd(sqrBracksOpen, OPERAND_INDEX_END_CHAR);
            if (*sqrBracksClose == '\0')
                return firstStageErr_operation_expected_closing_sqr_bracks;
            tokEnd = getStart(sqrBracksClose + 1);   /* there could be a space inside the brackets - update the token end */
            
            if (*tokEnd != ',' && *tokEnd != '\0')
                return firstStageErr_operation_invalid_operand;
            
            indexStart = getStart(sqrBracksOpen + 1);   /* skip the spaces between '[' and start of operand */
            if (indexStart == sqrBracksClose)
                return firstStageErr_operation_expected_index;

            /* get the last character of the index */
            for (indexEnd = sqrBracksClose - 1; isspace(*indexEnd); indexEnd--)
                ;

            if (getTokEnd(indexStart) < indexEnd || /* the index is not a single token */
                fetchNumber(indexStart, indexEnd + 1, &num, symbols) != firstStageErr_no_err)   /* can't read the number */
                return firstStageErr_operation_invalid_index;

            if (!validSymbolName(token, sqrBracksOpen))
                return firstStageErr_operation_invalid_label_name;

            if (!validAddressingMethod(operation, operandIndex, ADDR_CONSTANT_INDEX))
                return firstStageErr_operation_invalid_addr_method;
            
            operandAddrs[operandIndex] = ADDR_CONSTANT_INDEX;
            words[(*wordIndex)++].hasValue = 0;    /* skip the label's name */
            if (!writeImmediateToByte(&words[(*wordIndex)++], num) || num < 0)
                return firstStageErr_operation_index_oor;
        }
        else if (validSymbolName(token, tokEnd)) {
            /* check if the operand is a macro's name */
            temp = *tokEnd;
            *tokEnd = '\0';
            if (getMacroWithName(token, macros)) {
                *tokEnd = temp;
                return firstStageErr_operation_operand_macro;
            }
            *tokEnd = temp;
            
            /* the operand must be a direct (label) addressing */
            if (!validAddressingMethod(operation, operandIndex, ADDR_DIRECT))
                return firstStageErr_operation_invalid_addr_method;
            
            operandAddrs[operandIndex] = ADDR_DIRECT;
            words[(*wordIndex)++].hasValue = 0;
        }
        else {
            /* check if the token is an immediate number and print a warning if it is */
            temp = *tokEnd;
            *tokEnd = '\0';
            if (tryParseToken(token, &num)) {
                *tokEnd = temp;
                return firstStageErr_operation_operand_number;
            }
            
            *tokEnd = temp;
            return firstStageErr_operation_invalid_operand;
        }

        nextOperand:
        tokEnd = getStart(tokEnd);  /* skip spaces */
        if (*tokEnd != '\0' && *tokEnd != ',')
            return firstStageErr_operation_expected_comma;

        if (*tokEnd != '\0') {
            token = getStart(tokEnd + 1);
            if (*token == '\0' && *tokEnd == ',') {
                if (operandIndex < numOps - 1)
                    return firstStageErr_operation_expected_operand;
                else
                    return firstStageErr_operation_extra_chars;
            }
        }
        else
            token = tokEnd;
    }

    /* check for extra text following the operands */
    if (operandIndex < numOps)
        return firstStageErr_operation_expected_operand;
    
    return firstStageErr_no_err;
}
