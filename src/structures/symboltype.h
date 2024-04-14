#ifndef SYMBOLTYPE
#define SYMBOLTYPE

#define LABEL_MAX_LENGTH  31

#define SYMBOL_FLAG_MDEFINE 1
#define SYMBOL_FLAG_CODE    2
#define SYMBOL_FLAG_EXTERN  4
#define SYMBOL_FLAG_DATA    8
#define SYMBOL_FLAG_ENTRY   16

typedef struct symbol {
    char *name;
    int value;
    int flags;
    int length;
    struct symbol *next;
} Symbol;

int validSymbolName(char *start, char *end);
void addSymToList(Symbol **head, Symbol *symbol);
int symbolInList(Symbol *head, char *name);
int getSymbolByName(char *name, Symbol *head, Symbol **pSymbol);
Symbol *allocSymbol(char *nameStart, char *nameEnd);
void freeSymbolsList(Symbol *head);

#endif /* SYMBOLTYPE */
