#include <stdio.h>
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

int assemblerSecondStage(char fileName[], Symbol *symbols, ByteNode *bytes) {
    char *token, *tokEnd, *oprndEnd, temp;
    char sourceFileName[FILENAME_MAX], entFileName[FILENAME_MAX], extFileName[FILENAME_MAX];
    char line[MAXLINE + 1];
    unsigned int sourceLine, skippedLines;
    int instructionCounter, hasErr, operandIndex, index, addrsMethods[NUM_OPERANDS], hasEnt, hasExt;
    FILE *sourcef, *entf, *extf;
    Symbol *tempSym;
    Operation op;
    
    
    /* -- open files -- */
    sprintf(sourceFileName, "%s.%s", fileName, PRE_ASSEMBLED_FILE_EXTENSION);
    sprintf(entFileName, "%s.%s", fileName, ENTRIES_FILE_EXTENSION);
    sprintf(extFileName, "%s.%s", fileName, EXTERNALS_FILE_EXTENSION);

    openFile(sourceFileName, "r", &sourcef);
    openFile(entFileName, "w", &entf);
    openFile(extFileName, "w", &extf);
    
    sourceLine = 0;
    instructionCounter = INSTRUCTION_COUNTER_OFFSET;
    hasErr = 0;
    hasEnt = 0;
    hasExt = 0;
    while ((getNextLine(sourcef, line, 1, MAXLINE, &skippedLines)) != getLine_FILE_END) {
        sourceLine += skippedLines;
        
        token = getStart(line);
        
        /* label declaration? */
        if (*getTokEnd(token) == LABEL_END_CHAR)
            token = getNextToken(token);    /* skip to next token */
            
        /* .data, .string or .extern instruction? */
        if (tokcmp(token, KEYWORD_DATA_DEC) == 0 ||
            tokcmp(token, KEYWORD_STRING_DEC) == 0 ||
            tokcmp(token, KEYWORD_EXTERN_DEC) == 0)
            continue;   /* read next line */
            
        /* .entry instruction? */
        if (tokcmp(token, KEYWORD_ENTRY_DEC) == 0) {
            token = getNextToken(token);
            *(getTokEnd(token) + 1) = '\0'; /* this line will not be read anymore - no need to save the character's value */
            if (!getSymbolByName(token, symbols, &tempSym)) {
                printSecondStageErr(secondStageErr_entry_undefined, sourceLine, sourceFileName);
                hasErr = 1;
                continue;
            }
            
            /* was this symbol declared as extern? */
            if ((tempSym->flags & SYMBOL_FLAG_EXTERN) != 0) {
                printSecondStageErr(secondStageErr_entry_symbol_is_extern, sourceLine, sourceFileName);
                hasErr = 1;
                continue;
            }
            
            /* is the symbol a constant? */
            if ((tempSym->flags & SYMBOL_FLAG_MDEFINE) != 0) {
                printSecondStageErr(secondStageErr_entry_constant, sourceLine, sourceFileName);
                hasErr = 1;
                continue;
            }
            
            /* was this symbol already declared as entry? */
            if ((tempSym->flags & SYMBOL_FLAG_ENTRY) != 0) {
                logWarn("label '%s' was already declared as entry (in file \"%s\", line %u)\n", token, sourceFileName, sourceLine);
                continue;   /* skip to the next line */
            }
            
            tempSym->flags |= SYMBOL_FLAG_ENTRY; /* declare symbol as entry */
            fprintf(entf, "%d %s\n", tempSym->value, token);
            hasEnt = 1;
            continue;
        }
        
        if (!getOperationByName(token, &op))
            continue;   /* an error was already printed in the first stage */

        getAddrsMethods(addrsMethods, bytes->byte);
        
        instructionCounter++;
        bytes = bytes->next;
        token = getNextToken(token);    /* go to first operand */
        for (operandIndex = 0; operandIndex < NUM_OPERANDS; operandIndex++) {
            if (!operationHasOperand(op, operandIndex))
                continue;
            
            oprndEnd = tokEnd = getFirstOrEnd(token, ',');
            if (addrsMethods[operandIndex] == ADDR_CONSTANT_INDEX) {
                oprndEnd = getFirstOrEnd(token, OPERAND_INDEX_START_CHAR);
                oprndEnd = oprndEnd < tokEnd ? oprndEnd : tokEnd;
            }
            
            temp = *oprndEnd;
            *oprndEnd = '\0';
            
            if (addrsMethods[operandIndex] != ADDR_DIRECT && addrsMethods[operandIndex] != ADDR_CONSTANT_INDEX)
                goto nextOperand;
            
            if (!getSymbolByName(token, symbols, &tempSym)) {
                printSecondStageErr(secondStageErr_operation_symbol_undefined, sourceLine, sourceFileName);
                hasErr = 1;
                break;
            }
            
            /* is the symbol a constant? */
            if ((tempSym->flags & SYMBOL_FLAG_MDEFINE) != 0) {
                printSecondStageErr(secondStageErr_operation_symbol_constant, sourceLine, sourceFileName);
                hasErr = 1;
                break;
            }

            numberToByte(tempSym->value, &bytes->byte);
            shiftLeft(&bytes->byte, NUM_ARE_BITS);  /* shift left to make room for ARE bits */
            
            /* write to ARE bits */
            writeAREBits(&bytes->byte, tempSym->flags);
            
            /* is the symbol an extern? */
            if ((tempSym->flags & SYMBOL_FLAG_EXTERN) != 0) {
                fprintf(extf, "%d %s\n", instructionCounter, token);
                hasExt = 1;
            }
            else if (addrsMethods[operandIndex] == ADDR_CONSTANT_INDEX) {
                /* the operand isn't extern, we know its length */
                index = byteToNumber(bytes->next->byte, NUM_ARE_BITS);  /* fetch the index from the next byte */
                if (index < 0 || index >= tempSym->length) {
                    printSecondStageErr(secondStageErr_operation_index_oor, sourceLine, sourceFileName);
                    /* skip to next word */
                    bytes = bytes->next->next;
                    instructionCounter += 2;
                    
                    if (operandIndex != DEST_OPERAND_INDEX) {
                        bytes = bytes->next;
                        instructionCounter++;
                        
                        if (addrsMethods[DEST_OPERAND_INDEX] == ADDR_CONSTANT_INDEX) {
                            bytes = bytes->next;
                            instructionCounter++;
                        }
                    }
                    
                    hasErr = 1;
                    break;
                }
            }
            
            nextOperand:
            /* check if both are not registers */
            if (operandIndex == SOURCE_OPERAND_INDEX || addrsMethods[SOURCE_OPERAND_INDEX] != ADDR_REGISTER || addrsMethods[DEST_OPERAND_INDEX] != ADDR_REGISTER) {
                instructionCounter++;
                bytes = bytes->next;
            }
            
            if (addrsMethods[operandIndex] == ADDR_CONSTANT_INDEX) {
                instructionCounter++;
                bytes = bytes->next;    /* skip index byte */
            }
            
            *oprndEnd = temp;
            token = getStart(tokEnd + 1);
        }
    }
    
    /* close open files */
    fclose(sourcef);
    fclose(entf);
    fclose(extf);
    
    /* delete empty output files */
    if (!hasEnt)
        deleteFile(entFileName);
    if (!hasExt)
        deleteFile(extFileName);
        
    return hasErr;
}

static void printSecondStageErr(enum secondStageErr err, unsigned int lineNumber, char *fileName) {
    char *message;

    message = getErrMsg(err);
    logErr("file \"%s\" line %u - %s\n", fileName, lineNumber, message);
}

static char *getErrMsg(enum secondStageErr err) {
    switch (err) {
        /* -- entry -- */
        case secondStageErr_entry_undefined:
            return "undefined label for .entry argument";
            
        case secondStageErr_entry_constant:
            return "can't define a constant as entry";
        
        case secondStageErr_entry_symbol_is_extern:
            return "symbol was already defined as extern";
            
            
        /* -- operation -- */
        case secondStageErr_operation_symbol_constant:
            return "constant operand must follow #";
            
        case secondStageErr_operation_symbol_undefined:
            return "undefined symbol";
            
        case secondStageErr_operation_index_oor:
            return "index out of range";
            
        default:
            return "UNDEFINED ERROR";
    }
}
