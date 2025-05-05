#ifndef _XSLEXER_H
#define _XSLEXER_H
#include "xsdef.h"


void initLex(_xsvm *vm);
int xs_ignoreSpace(_xsvm *vm);
int ignoreComments(_xsvm *vm);
char* nextToken(_xsvm *vm);

#endif
