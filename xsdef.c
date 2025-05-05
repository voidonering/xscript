#include "xsdef.h"
#include "xsvm.h"
jmp_buf Jump_Buffer;
void handleException(_xsvm *vm, int eid)
{
	vm->lastError=eid;
	longjmp(Jump_Buffer, 1);
}
