#include "xsiolib.h"
#include "xsutil.h"

void io_print(_xsvm *vm)
{
	_variable v=xs_popValue(vm);
	if(v.type==dtNumber)
	{
		printf("%f",v.v.Number);
	}
	else if(v.type==dtString)
	{
		printf("%s",v.v.String);
	}
	else
	{
		printf("[Object]");
	}
}

void io_getc(_xsvm *vm)
{
	printf("\npress Enter to continue...\n");
	getchar();
}

void io_fileopen()
{
	
	//FILE *f=fopen(fn, "rb+wb");
	
}

void io_filecreate()
{
	
}

void io_filedelete()
{
	
}

void io_fileread()
{
	
}

void io_filewrite()
{
	
}

void io_fileseek()
{
	
}

void xs_deleteMemory(xsuint o)
{
	free( (char*)o);
}
void io_readFile(_xsvm *vm)
{
	char *filename=xs_popString(vm);
	xsuint size;
	char *s=xs_readFile(filename, &size);
	xs_pushObject(vm, (xsuint)s, xs_deleteMemory);
	free(filename);
	//free(s);
}

void io_getFileSize(_xsvm *vm)
{
	char *filename=xs_popString(vm);
	int size=xs_getFileSize(filename);
	free(filename);
	xs_pushNumber(vm, size);
}

void io_extractFilePath(_xsvm *vm)
{
	char *filename=xs_popString(vm);
	char path[256];
	xs_extractFilePath(filename, path);
	free(filename);
	xs_pushString(vm, path);
}

void useiolib(_xsvm *vm)
{
	xs_register(vm, "io_getc",0,io_getc);
	xs_register(vm, "io_print",1,io_print);
	xs_register(vm, "io_fileopen",1,io_print);
	xs_register(vm, "io_filecreate",1,io_print);
	xs_register(vm, "io_filedelete",1,io_print);
	xs_register(vm, "io_fileread",1,io_print);
	xs_register(vm, "io_filewrite",1,io_print);
	xs_register(vm, "io_fileseek",1,io_print);
	xs_register(vm, "io_readFile",1,io_readFile);
	xs_register(vm, "io_getFileSize",1,io_getFileSize);
	
	xs_register(vm, "io_extractFilePath",1,io_extractFilePath);
}

