#include "xslog.h"
#include "stdio.h"
#include "xsapi.h"

FILE* xsfh=0;
char *xslogfile="c:\\data\\xslog.txt";

void xs_log(char *msg)
{
	xsfh=fopen(xslogfile,"a");
	if(xsfh==NULL )
	{
		//printf("Can not open file for write: %s, press Enter to exit...",xslogfile);
		//getchar();
		return;
	}
	fwrite(msg,1,strlen(msg),xsfh);
	fwrite("\r\n",1,2,xsfh);
	fclose(xsfh);
}

void xs_freelog()
{
	return;	
}

void debug_log(_xsvm *vm)
{
	char *msg=xs_popString(vm);
	xs_log(msg);
	free(msg);
}

void xs_useLog(_xsvm *vm)
{
	xs_register(vm,"debug_log",  1, debug_log);
}

