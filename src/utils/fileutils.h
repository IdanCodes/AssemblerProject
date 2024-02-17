#ifndef FILEUTILS
#define FILEUTILS

void openFile(char fileName[], char mode[], FILE **pfile);
int tryOpenFile(char fileName[], char mode[], FILE **pfile);

#endif /* FILEUTILS */
