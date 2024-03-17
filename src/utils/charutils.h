#ifndef CHARUTILS
#define CHARUTILS

#include <ctype.h>

#define COMMENT_CHAR    ';' /* starts a comment */
#define LABEL_END_CHAR       ':' /* ends a label definition */
#define IMMEDIATE_OPERAND_PREFIX_CHAR    '#' /* prefix for an immediate operand */
#define OPERAND_INDEX_START_CHAR '[' /* the opening character for an index specifier in an operand */
#define OPERAND_INDEX_END_CHAR   ']' /* the closing character for an index specifier in an operand */

#endif /* CHARUTILS */
