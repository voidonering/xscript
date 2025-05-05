
#include "xs.h"
#include "xscodegen.h"
#include "xslog.h"
#include "xsvm.h"


xsbyte xs_onEvent(_xsvm *vm, XS_NUMBER sender, XS_NUMBER eid, XS_NUMBER e)
{
	xs_pushNumber(vm,sender);
	xs_pushNumber(vm,eid);
	xs_pushNumber(vm,e);
	
	return xs_onScriptEvent(vm);
}

/*
void xs_onUpdate(_xsvm *vm)
{
	xs_onScriptUpdate(vm );
}
*/

//±ØÐëÊÍ·Å
char* xs_getStringVariable(_xsvm *vm, char *varName)
{
	_object *o=vm->globalObject;

	int j;
	char *n;
	for( j=0; j<o->varCount; j++ )
	{
		_variable *v=&o->variables[j];//stackIndex(&o->variables,j);
		n=xs_getVarNameByAddr(vm,v->name);
		if(n)
		{
			if(strcmp(n+3, varName)==0)
			{
				char *p=xs_toString(v);
				return p;
			}			
		}
	}		
	
	return NULL;
}

XS_NUMBER xs_getNumberVariable(_xsvm *vm, char *varName)
{
	_object *o=vm->globalObject;

	int j;
	char *n;
	for( j=0; j<o->varCount; j++ )
	{
		_variable *v=&o->variables[j];//stackIndex(&o->variables,j);
		n=xs_getVarNameByAddr(vm,v->name);
		if(n)
		{
			if(strcmp(n+3, varName)==0)
			{
				return xs_toNumber(v);
			}			
		}
	}		
	
	return 0;
}


void xs_setNumberVariable(_xsvm *vm, char *varName, XS_NUMBER number)
{
	_object *o=vm->globalObject;

	int j;
	char *n;
	for( j=0; j<o->varCount; j++ )
	{
		_variable *v=&o->variables[j];//stackIndex(&o->variables,j);
		n=xs_getVarNameByAddr(vm,v->name);
		if(n)
		{
			if(strcmp(n+3, varName)==0)
			{
				v->v.Number=number;
				break;
			}			
		}
	}		
	
}

