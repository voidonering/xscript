#ifndef _XSGC_H
#define _XSGC_H
#include "xsdef.h"

#ifdef __cplusplus
extern "C" {
#endif

void xs_addRef(_xsvm *vm, xsuint o);
int xs_removeRef(_xsvm *vm, xsuint o);
void xs_gcAddObject(_xsvm *vm, xsuint o,disposeapi dispose);
struct hashtable * xs_createGc();

#ifdef __cplusplus
}
#endif

#endif
