#ifndef SYMBOL_TYPE
#define SYMBOL_TYPE

#define LABEL_MAX_LENGTH  31

#define SYMBOL_FLAG_MDEFINE 1
#define SYMBOL_FLAG_CODE    2
#define SYMBOL_FLAG_EXTERN  4
#define SYMBOL_FLAG_DATA    8

/* TODO: when accessing by index, check if the range of the index is bad out of bounds */
typedef struct sym {
    char *name;
    int value;
    int flag;
    int length;
    struct sym *next;
} Symbol;

int validSymbolName(char *start, char *end);
void addSymToList(Symbol **head, Symbol *symbol);
int symbolInList(Symbol *head, char *name);
int getSymbolByName(char *name, Symbol *head, Symbol **pSymbol);
Symbol *allocSymbol(char *nameStart, char *nameEnd);
void freeSymbolsList(Symbol *head);

#endif /* SYMBOL_TYPE */
