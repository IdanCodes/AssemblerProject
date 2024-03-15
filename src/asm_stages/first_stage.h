#ifndef FIRST_STAGE
#define FIRST_STAGE

/* make a seperate file to store keyword definitions */
#define LABEL_MAX_LENGTH  31
#define INSTRUCTION_COUNTER_OFFSET  100

#define SYMBOL_FLAG_MDEFINE 1
#define SYMBOL_FLAG_CODE    2
#define SYMBOL_FLAG_EXTERN  4
#define SYMBOL_FLAG_DATA    8

/* when accessing by index, check if the range of the index is bad out of bounds */
typedef struct Symbol {
    char *name;
    int value;
    int flag;
    struct Symbol *next;
} Symbol;

enum firstStageErr {
    firstStageErr_no_err,   /* no error */
    
    /* .define errors */
    firstStageErr_define_name_expected, /* name expected */
    firstStageErr_define_unexpected_chars,  /* unexpected chars at the end of a define statement */
    firstStageErr_define_expected_equal_sign,   /* expected after constant name */
    firstStageErr_define_value_expected,    /* expected value */
    firstStageErr_define_invalid_name,  /* the constant's name was invalid */
    firstStageErr_define_saved_keyword, /* the constant's name is a saved keyword */
    firstStageErr_define_name_taken,    /* name of constant taken */
    firstStageErr_define_value_nan, /* value is not a number */
    
    /* label errors */
    firstStageErr_label_invalid_name,   /* invalid label name */
    firstStageErr_label_const_definition,   /* constant defined in a label */
    firstStageErr_label_saved_keyword,  /* the label's name is a saved keyword */
    firstStageErr_label_name_taken, /* name of label taken */
    firstStageErr_label_empty_line, /* label defined on an empty line */
    
    /* .data errors */
    firstStageErr_data_nan, /* data parameter is not a number */
    firstStageErr_data_comma_expected,  /* comma expected between arguments */
    firstStageErr_data_const_not_found, /* constant referenced was not found */
    firstStageErr_data_argument_expected,   /* argument expected in data */
    
    /* .string errors */
    firstStageErr_string_expected_quotes,   /* quotes missing after .string */
    firstStageErr_string_expected_end_quotes,   /* quotes missing at end of .string definition */
    firstStageErr_string_extra_chars,   /* extra characters after closing quotes */
    
    /* .extern errors */
    firstStageErr_extern_invalid_lbl_name,  /* the .extern parameter is not a valid name for a label */ 
    firstStageErr_extern_extra_chars,   /* extra characters at the end of an extern instruction */
    firstStageErr_extern_def_label_same_name,   /* defining label with the same name as the .extern parameter */
    firstStageErr_extern_label_exists,  /* the label is already defined in the file */
    firstStageErr_extern_saved_keyword, /* extern label is a saved keyword */
    
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
    firstStageErr_operation_extra_chars /* extra characters at the end of the line */
};

void assemblerFirstStage(char fileName[]);
int validSymbolName(char *start, char *end);

#endif /* FIRST_STAGE */
