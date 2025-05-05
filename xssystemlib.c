/*
2009/03/09 TangJun
standard system library
including some base functions
*/
#include <stdlib.h>
#include <time.h>
#include "xssystemlib.h"
#include "xsvm.h"
#include "xscodegen.h"

//取得系统时钟，毫秒级
void system_clock(_xsvm *vm)
{
	//xs_pushNumber(vm, clock());
}

//编译执行一段脚本
void system_run(_xsvm *vm)
{
	char *scr=xs_popString(vm);
	int index,j;
	_object o;
	_object *co;
	
	index=xs_addConstString(vm,"$dobject")	;
	memset(&o,0,sizeof(_object));
	o.name=index;
	o.typeName=index;
	o.parent=vm->currentObject;
	o.type=dtFunction;
	//stackNew(&o.variables,sizeof(_variable),2);
	//stackNew(&o.code,sizeof(_instruction),10);
			
	co=vm->currentObject;
	vm->currentObject=&o;
	if(xs_compile(vm, scr)==XS_SUCCESS)
	{
		vm->currentObject=co;
		xs_callFunction(vm, &o);
	}
	
	for(j=0; j<o.varCount; j++ )
	{
		xs_freeVariable(vm,&o.variables[j]);
	}
	
	//stackFree(&o.variables);
	//stackFree(&o.code);	
	free(scr);
}

//int color=rgb(r, g, b)
void system_rgb(_xsvm *vm)
{
	unsigned int b=(unsigned int)xs_popNumber(vm);
	unsigned int g=(unsigned int)xs_popNumber(vm);
	unsigned int r=(unsigned int)xs_popNumber(vm);
	
	unsigned int c=(r<<16)+(g<<8)+b;
	xs_pushNumber(vm, c);
}

void system_argb(_xsvm *vm)
{
	unsigned int b=(unsigned int)xs_popNumber(vm);
	unsigned int g=(unsigned int)xs_popNumber(vm);
	unsigned int r=(unsigned int)xs_popNumber(vm);
	unsigned int a=(unsigned int)xs_popNumber(vm);
		
	unsigned int c=(a<<24)+(r<<16)+(g<<8)+b;
	xs_pushNumber(vm, c);
}

void system_getBasePath(_xsvm *vm)
{	
	xs_pushString(vm, vm->workingPath);
}

void usesystemlib(_xsvm *vm)
{
    //xs_register(vm, "sys_clock", 0,  system_clock);
    xs_register(vm, "sys_run", 1,  system_run);
    
    xs_register(vm, "sys_rgb", 3, system_rgb);
    xs_register(vm, "sys_argb", 4, system_argb);
    xs_register(vm, "sys_getBasePath",0, system_getBasePath);
}
