#ifndef FIRST_STAGE
#define FIRST_STAGE

/* make a seperate file to store keyword definitions */
#define LABEL_MAX_SIZE  31
#define KEYWORD_CONSTANT_DEFINITION  ".define"

typedef struct AsmConst {
    char *name;
    double value;
    struct AsmConst *next;
} AsmConst;

enum firstStageErr {
    firstStageErr_no_err,   /* no error */
    firstStageErr_unexpected_chars_define,
    firstStageErr_expected_equal_sign_define,   /* expected after constant name */
    firstStageErr_invalid_name_define,  /* the constant name was invalid */
    firstStageErr_name_taken_define,    /* name of constant taken */
    firstStageErr_const_value_nan   /* value is not a number */
};

void assemblerFirstStage(char fileName[]);
void constDefine(char line[]);
int validLabelName(char *name);

#endif /* FIRST_STAGE */
