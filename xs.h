#ifndef _XS_H_
#define _XS_H_
#include "xsdef.h"
#include "xsvm.h"

#ifdef __cplusplus
extern "C" {
#endif

_xsvm* xs_init(char *wd);
void xs_free(_xsvm *vm );
int xs_run(_xsvm *vm );
int xs_compile(_xsvm *vm,   char* script);
int xs_decompile(_xsvm *vm, char* dst);
int xs_deCompileToFile(_xsvm *vm, char *filename);
//void xs_onEvent(_xsvm *vm);//, XS_NUMBER Sender,XS_NUMBER eventID, XS_NUMBER e);
xsbyte xs_onEvent(_xsvm *vm, XS_NUMBER sender, XS_NUMBER eid, XS_NUMBER e);
//#define xs_onEvent xs_onScriptEvent
//void xs_onUpdate(_xsvm *vm);
#define xs_onUpdate xs_onScriptUpdate

void xs_pushNumber(_xsvm *vm, XS_NUMBER n);
void xs_pushString(_xsvm *vm, char* p);
/*
xs_pushObject
必须是新创建的对象,每个对象只能用此函数push一次,push后即进行自动管理,无需手动释放
*/
void xs_pushObject(_xsvm *vm, xsuint o,disposeapi dispose);
//void xs_pushObject(void* o);

XS_NUMBER xs_popNumber(_xsvm *vm );
char* xs_popString(_xsvm *vm );
xsuint xs_popObject(_xsvm *vm );
_variable xs_popValue(_xsvm *vm);

void xs_register(_xsvm *vm, char *api_name,int paramCount,xsapi api);

char* xs_getStringVariable(_xsvm *vm, char *varName);
XS_NUMBER xs_getNumberVariable(_xsvm *vm, char *varName);
void xs_setNumberVariable(_xsvm *vm, char *varName, XS_NUMBER number);

#ifdef __cplusplus
}
#endif

#endif
