#ifndef _XSVM_H
#define _XSVM_H
#include "xsdef.h"
#include "xslexer.h"
#include "xsparser.h"
#include "xscodegen.h"
#include "stdio.h"
#include "xsapi.h"

#ifdef __cplusplus
extern "C" {
#endif

void xs_assign(_xsvm *vm, _variable *vl,_variable *vr);
/*
#define xs_assign(vl,vr)												\
	xs_freeVariable(vl);												\
	vl->type=vr->type;													\
	vl->v=vr->v;														\
	if(vl->type==dtString)												\
	{																	\
		vl->v.String=malloc(strlen(vl->v.String)+1);					\
		strcpy(vl->v.String,vm.globalStack[vm.stackCount].v.String);	\
	}																	\
	else if(vl->type==dtArray)											\	
	{																	\
		_stack *s=calloc(1,sizeof(_stack));								\
		_stack *sr=(_stack*)vr->v.Int;									\
		stackNew(s,sizeof(_variable),sr->count);						\
		vl->v.Int=(int)s;												\
		stackCopy(s,sr);												\
	}																	
*/

//_xsvm* xs_initVM(char *wd);
//void xs_freeVM(_xsvm *vm);
void xs_loadLibs(_xsvm *vm);
//int xs_runScript(_xsvm *vm);
//int xs_compileScript(_xsvm *vm,  char* script);
//int step(  _instruction *ins, int *nextCP);
int xs_steps(_xsvm *vm);
void xs_callFunction(_xsvm *vm,_object* o);


void setArrayValue(_xsvm *vm,_variable *vl,XS_NUMBER index, _variable *vr);
void getArrayValue(_xsvm *vm,_variable *vl,XS_NUMBER index, _variable *vr );

int xs_addVarName(_xsvm *vm, char* name);

int xs_addConst(_xsvm *vm,  _variable d);

int xs_addConstString( _xsvm *vm, char* name);
/*
int addFunction(  char* name)
{
	//是否已定义，若已定义，抛出错误
	int index=addConstString(vm,name);

	_function f;
	//memset(&f,0,sizeof(_function));
	f.name=index;
	f.code=listNew(sizeof(_instruction),10);
	f.parent=vm->currentFunction;

	return listAdd(vm->functionVector,&f);
	
}
*/
int xs_addObject(_xsvm *vm,  char* name, xsbyte type);
int xs_newObject(_xsvm *vm,  unsigned short name);
void xs_makeVarName(_xsvm *vm,  _object *owner, char *vn, char* newVN);
//void xs_getObject(  char* name,_object *o);
//void xs_getObjectByName(  char* name);
int xs_objectExists(_xsvm *vm, unsigned short name, int *oi);

int xs_addVar(_xsvm *vm, _object *o, char* name);
int xs_getVarAddrByName(_xsvm *vm, _object *o, char* name,int *layer, int *idx);
char* xs_getVarNameByAddr(_xsvm *vm, int index);

void xs_freeVariable(_xsvm *vm,_variable *v);
void xs_freeArray(_xsvm *vm,_variable *v);
void xs_convertToString(_xsvm *vm,_variable *v);
char* xs_toString(_variable *v);
double xs_toNumber(_variable *v);

xsbyte xs_onScriptEvent(_xsvm *vm );
void xs_onScriptUpdate(_xsvm *vm);
char* xs_getNewObjectName(_xsvm *vm);
//void xs_setVarValue(_object *o, int index, byte varType, _value var );

#ifdef __cplusplus
}
#endif
#endif
