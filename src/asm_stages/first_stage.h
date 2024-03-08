#ifndef FIRST_STAGE
#define FIRST_STAGE

/* make a seperate file to store keyword definitions */
#define LABEL_MAX_LENGTH  31

#define SYMBOL_FLAG_MDEFINE 1
#define SYMBOL_FLAG_EXTERN 2

/* TODO: does a space have to come after a label definition? */
typedef struct Symbol {
    char *name;
    int value;
    int flag;
    struct Symbol *next;
} Symbol;

enum firstStageErr {
    firstStageErr_no_err,   /* no error */
    
    /* .define errors */
    firstStageErr_name_expected_define, /* name expected */
    firstStageErr_unexpected_chars_define,  /* unexpected chars at the end of a define statement */
    firstStageErr_expected_equal_sign_define,   /* expected after constant name */
    firstStageErr_value_expected_define,    /* expected value */
    firstStageErr_invalid_name_define,  /* the constant's name was invalid */
    firstStageErr_saved_keyword_define, /* the constant's name is a saved keyword */
    firstStageErr_name_taken_define,    /* name of constant taken */
    firstStageErr_value_nan_define, /* value is not a number */
    
    /* label errors */
    firstStageErr_invalid_name_label,   /* invalid label name */
    firstStageErr_const_defined_in_label,   /* constant defined in a label */
    firstStageErr_saved_keyword_label,  /* the label's name is a saved keyword */
    firstStageErr_name_taken_label, /* name of label taken */
    firstStageErr_label_empty_line, /* label defined on an empty line */
    
    /* .data errors */
    firstStageErr_data_nan, /* data parameter is not a number */
    firstStageErr_data_comma_expected,  /* comma expected between arguments */
    firstStageErr_data_const_not_found, /* constant referenced was not found */
    
    /* .string errors */
    firstStageErr_string_expected_quotes,   /* quotes missing after .string */
    firstStageErr_string_expected_end_quotes,   /* quotes missing at end of .string definition */
    firstStageErr_string_extra_chars,   /* extra characters after closing quotes */
    
    /* .extern errors */
    firstStageErr_extern_invalid_lbl_name,  /* the .extern parameter is not a valid name for a label */ 
    firstStageErr_extern_extra_chars,   /* extra characters at the end of an extern instruction */
    firstStageErr_extern_def_label_same_name,   /* defining label with the same name as the .extern parameter */
    firstStageErr_extern_label_exists,  /* the label is already defined in the file */
    firstStageErr_extern_saved_keyword
};

void assemblerFirstStage(char fileName[]);
int validSymbolName(char *tok, char *end);

#endif /* FIRST_STAGE */
