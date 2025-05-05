#include "xsdef.h"
#include "xsapi.h"
#include "xsvm.h"
#include "xsgc.h"

/*popString(), 弹出的字符串要调用者释放
 *pushString(), 内部会复制字符串,所以调用者也要释放 
 * 
 * 
 */
void xs_pushNumber(_xsvm *vm, XS_NUMBER n)
{
	if( vm->stackCount>=maxStackCount ) xs_throw(VE_STACK_OVERFLOW);

    vm->globalStack[vm->stackCount].type=dtNumber;
	vm->globalStack[vm->stackCount].v.Number=n;
	vm->stackCount++;
}

void xs_pushString(_xsvm *vm, char* p)
{
	char *buf;
	if( vm->stackCount>=maxStackCount ) xs_throw(VE_STACK_OVERFLOW);
	buf=(char*)calloc(strlen(p)+1,1);
	strcpy(buf,p);
    vm->globalStack[vm->stackCount].type=dtString;
	vm->globalStack[vm->stackCount].v.String=buf;
	vm->stackCount++;
}

void xs_pushObject2(_xsvm *vm, xsuint o)
{
	if( vm->stackCount>=maxStackCount ) xs_throw(VE_STACK_OVERFLOW);
    vm->globalStack[vm->stackCount].type=dtObject;
	vm->globalStack[vm->stackCount].v.Number=(XS_NUMBER)o;
	vm->stackCount++;	
}

void xs_pushObject(_xsvm *vm, xsuint o,disposeapi dispose)
{
	xs_gcAddObject(vm, o,dispose);
	xs_pushObject2(vm, o);
}

XS_NUMBER xs_popNumber(_xsvm *vm)
{
	if(vm->stackCount<1 ) xs_throw(VE_STACK_ERROR);
	vm->stackCount--;
	if(vm->globalStack[vm->stackCount].type!=dtNumber ) xs_throw(VE_STACK_NOTNUMBER);
	return vm->globalStack[vm->stackCount].v.Number;
}

char* xs_popString(_xsvm *vm)
{
	if(vm->stackCount<1 ) xs_throw(VE_STACK_OVERFLOW);
	vm->stackCount--;
	if(vm->globalStack[vm->stackCount].type!=dtString ) xs_throw(VE_STACK_NOTSTRING);
	return vm->globalStack[vm->stackCount].v.String;
}
xsuint xs_popObject(_xsvm *vm)
{
	XS_OBJECT obj;
	if(vm->stackCount<1 ) xs_throw(VE_STACK_OVERFLOW);
	vm->stackCount--;
	if(vm->globalStack[vm->stackCount].type!=dtObject ) xs_throw(VE_STACK_NOTOBJECT);
	obj=(XS_OBJECT)vm->globalStack[vm->stackCount].v.Number;
	xs_freeVariable(vm, &vm->globalStack[vm->stackCount]);
	
	return obj;	
}

_variable xs_popValue(_xsvm *vm)
{
	if(vm->stackCount<1 ) xs_throw(VE_STACK_OVERFLOW);
	vm->stackCount--;
	return vm->globalStack[vm->stackCount];		
}

void xs_register(_xsvm *vm, char *api_name,int paramCount,xsapi api)
{
	_xsapi a;
	if( strlen(api_name)>=maxAPINameLength )
	{
		xs_throw(VE_API_APINAMETOOLONG);		
	}

	strcpy(a.apiName,api_name);
	a.api=api;
	a.paramCount=paramCount;

	stackPush(&vm->apiStack,&a);
}

