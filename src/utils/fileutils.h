#ifndef FILEUTILS
#define FILEUTILS

/* file endings */
#define SOURCE_FILE_EXTENSION           "as"
#define PRE_ASSEMBLED_FILE_EXTENSION    "am"
#define ENTRIES_FILE_EXTENSION          "ent"
#define EXTERNALS_FILE_EXTENSION        "ext"
#define OBJECT_FILE_EXTENSION           "obj"

void openFile(char fileName[], char mode[], FILE **pfile);
int tryOpenFile(char fileName[], char mode[], FILE **pfile);
void deleteFile(char fileName[]);
int tryDeleteFile(char fileName[]);

#endif /* FILEUTILS */
