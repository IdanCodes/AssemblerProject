#ifndef FIRST_STAGE
#define FIRST_STAGE

#include "../structures/bytelist.h"
#include "../structures/symboltype.h"
#include "../structures/macrotype.h"

#define INSTRUCTION_COUNTER_OFFSET  100

enum firstStageErr {
    firstStageErr_no_err,   /* no error */
    
    /* .define errors */
    firstStageErr_define_name_expected, /* name expected */ /* DONE */
    firstStageErr_define_unexpected_chars,  /* unexpected chars at the end of a define statement */ /* DONE */
    firstStageErr_define_expected_equal_sign,   /* expected after constant name */  /* DONE */
    firstStageErr_define_value_expected,    /* expected value for constant */   /* DONE */
    firstStageErr_define_invalid_name,  /* the constant's name was invalid */   /* DONE */
    firstStageErr_define_saved_keyword, /* the constant's name is a saved keyword */    /* DONE */
    firstStageErr_define_name_taken,    /* name of constant taken */    /* DONE */
    firstStageErr_define_value_nan, /* value is not a number */ /* DONE */
    firstStageErr_define_macro_name,    /* there's a macro with the same name as the constant */    /* DONE */
    
    /* label errors */
    firstStageErr_label_invalid_name,   /* invalid label name */    /* DONE */
    firstStageErr_label_const_definition,   /* constant defined in a label */   /* DONE */
    firstStageErr_label_saved_keyword,  /* the label's name is a saved keyword */   /* DONE */
    firstStageErr_label_name_taken, /* name of label taken */   /* DONE */
    firstStageErr_label_empty_line, /* label defined on an empty line */    /* DONE */
    firstStageErr_label_macro_name, /* there's a macro with the same name as the label */   /* DONE */
    
    /* .data errors */
    firstStageErr_data_nan, /* .data argument is not a number */    /* DONE */
    firstStageErr_data_comma_expected,  /* comma expected between arguments */  /* DONE */
    firstStageErr_data_const_not_found, /* constant referenced was not found */ /* DONE */
    firstStageErr_data_argument_expected,   /* argument expected in data */ /* DONE */
    firstStageErr_data_oor, /* the data argument was out of range for a byte */ /* DONE */
    
    /* .string errors */
    firstStageErr_string_expected_quotes,   /* quotes missing after .string */  /* DONE */
    firstStageErr_string_expected_end_quotes,   /* quotes missing at end of .string definition */   /* DONE */
    firstStageErr_string_extra_chars,   /* extra characters after closing quotes */ /* DONE */
    firstStageErr_string_not_printable, /* a character in the string is not printable */    /* DONE */
    
    /* .extern errors */
    firstStageErr_extern_invalid_lbl_name,  /* the .extern parameter is not a valid name for a label */ /* DONE */
    firstStageErr_extern_extra_chars,   /* extra characters at the end of an extern instruction */  /* DONE */
    firstStageErr_extern_label_exists,  /* the label is already defined in the file */  /* DONE */
    firstStageErr_extern_saved_keyword, /* extern label is a saved keyword */   /* DONE */
    firstStageErr_extern_define_label,  /* a label is defined in the beginning of an extern instruction (warning) */    /* DONE */
    firstStageErr_extern_exists, /* this label was already declared as extern (warning) */  /* DONE */
    firstStageErr_extern_macro_name,    /* there's a macro with the same name as the .extern argument */    /* DONE */
    
    /* .entry errors */
    firstStageErr_entry_invalid_lbl_name,   /* the .entry argument is not a valid name for a label */   /* DONE */
    firstStageErr_entry_extra_chars,    /* extra characters at the end of an entry instruction */   /* DONE */
    firstStageErr_entry_define_label,   /* a label is defined in the beginning of an entry instruction (warning) */
    
    /* operation errors */
    firstStageErr_operation_not_found,  /* invalid operator name */
    firstStageErr_operation_expected_operand,   /* expected an operand */
    firstStageErr_operation_invalid_immediate,  /* the immediate operand was invalid */
    firstStageErr_operation_expected_closing_sqr_bracks,    /* expected closing square brackets */
    firstStageErr_operation_expected_index, /* index was not found between square brackets */
    firstStageErr_operation_invalid_index,  /* the given index (in square brackets) was invalid */
    firstStageErr_operation_invalid_label_name, /* label name in operand has an invalid name */
    firstStageErr_operation_invalid_addr_method,    /* invalid addressing method */
    firstStageErr_operation_invalid_operand,    /* operand expected for operation */
    firstStageErr_operation_expected_comma, /* expected comma to seperate operands */
    firstStageErr_operation_immediate_oor,  /* immediate operand was out of range */
    firstStageErr_operation_index_oor,  /* constant index was out of range */
    firstStageErr_operation_too_many_operands, /* too many operands for an operation */
    firstStageErr_operation_extra_chars,    /* extra characters at the end of the line */
    firstStageErr_operation_operand_number, /* a number operand that doesn't follow # */
    firstStageErr_operation_operand_macro   /* a macro was used as an operand */ 
};

int assemblerFirstStage(char fileName[], int **data, Macro *macros, Symbol **symbols, ByteNode **bytes, int *instructionCounter, int *dataCounter);

#endif /* FIRST_STAGE */
