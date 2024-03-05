#ifndef FIRST_STAGE
#define FIRST_STAGE

/* make a seperate file to store keyword definitions */
#define LABEL_MAX_LENGTH  31

/* TODO: change to symbol with flags (mdefine flag for constant) */
typedef struct Symbol {
    char *name;
    int value;
    int mdefine;
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
    firstStageErr_invalid_name_label
};

void assemblerFirstStage(char fileName[]);
int validSymbolName(char *tok, char *end);

#endif /* FIRST_STAGE */
