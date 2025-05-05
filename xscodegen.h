#ifndef _XSCODEGEN_H
#define _XSCODEGEN_H
#include "xsdef.h"


#ifdef __cplusplus
extern "C" {
#endif
	
	
int xs_addCode(_xsvm *vm,int varAddr,xsbyte dataScope, xsbyte opCode);
void xs_getInsString(_xsvm *vm, _object *o, _instruction *ins,char *buf);
//int xs_deCompile(_xsvm *vm,char* dst);
char* xs_getOpString(xsbyte op);
void xs_printVariables(_xsvm *vm,char *dst);
void xs_printEnv(_xsvm *vm);
void xs_getGlobalVariables(_xsvm *vm, char *dst);
void xs_getLocalVariables(_xsvm *vm, char *dst);

#ifdef __cplusplus
}
#endif


#endif
