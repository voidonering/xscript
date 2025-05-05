
#ifndef _XSIOLIB_H
#define _XSIOLIB_H
#include "xsapi.h"


#ifdef __cplusplus
extern "C" {
#endif
	

void useiolib(_xsvm *vm);

void io_print(_xsvm *vm);
void io_getc(_xsvm *vm);


#ifdef __cplusplus
}
#endif

#endif
