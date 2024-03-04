#ifndef FIRST_STAGE
#define FIRST_STAGE

/* make a seperate file to store keyword definitions */
#define LABEL_MAX_SIZE  31
#define KEYWORD_CONSTANT_DEFINITION  ".define"

/* TODO: change to symbol with flags (mdefine flag for constant) */
union SymbolType {
    double value;
    unsigned int dataAddress;
};

typedef struct Symbol {
    char *name;
    union SymbolType *value;
    int mdefine;
    struct Symbol *next;
} Symbol;

enum firstStageErr {
    firstStageErr_no_err,   /* no error */
    firstStageErr_name_expected_define, /* name expected */
    firstStageErr_unexpected_chars_define,
    firstStageErr_expected_equal_sign_define,   /* expected after constant name */
    firstStageErr_invalid_name_define,  /* the constant name was invalid */
    firstStageErr_name_taken_define,    /* name of constant taken */
    firstStageErr_value_nan_define  /* value is not a number */
};

void assemblerFirstStage(char fileName[]);
void constDefine(char line[]);
int validLabelName(char *name);

#endif /* FIRST_STAGE */
