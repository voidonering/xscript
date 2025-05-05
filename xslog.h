#ifndef _XSLOG_H
#define _XSLOG_H
#include "xsdef.h"

#ifdef __cplusplus
extern "C" {
#endif

extern FILE* xsfh;
extern char *xslogfile;

void xs_log(char *msg);
void xs_freelog();
void xs_useLog(_xsvm *vm);

#ifdef __cplusplus
}
#endif

#endif


