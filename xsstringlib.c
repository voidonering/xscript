/*
2009/03/09 TangJun
standard string library
*/

#include "xsstringlib.h"
#include "string.h"
#include "xsutil.h"

void string_length(_xsvm *vm)
{
	char *str=xs_popString(vm);
	int len=strlen( str );
	xs_pushNumber(vm,len);
	
	free(str);
}

void string_toLower(_xsvm *vm)
{
	char *str=xs_popString(vm);
	//strlwr( str );
	xs_pushString(vm,  str);
	free(str);
}

void string_toUpper(_xsvm *vm)
{
	char *str=xs_popString(vm);
	//strupr( str );
	xs_pushString(vm, str );
	free(str);
}

//substring(str, 0, 1 ); "abcd"
void string_subString(_xsvm *vm)
{
	int len=xs_popNumber(vm);
	int start=xs_popNumber(vm);
	char *src=xs_popString(vm);
	
	int srclen=strlen(src);
	if( start>=0 && start<srclen )
	{
		char *buf;
		len=len<0?1:len;
		len=len>(srclen-start)?(srclen-start):len;
		buf=(char*)calloc( len+1, 1);
		memcpy(buf, src+start, len);
		
		xs_pushString(vm, buf);
		free(buf);
	}
	else
	{
		xs_pushString(vm, " ");
	}
}

//indexof(str, sub );
void string_indexOf(_xsvm *vm)
{
	char *sub=xs_popString(vm);
	char *src=xs_popString(vm);
	char *f=strstr(src,sub);
	int i=-1;
	if( f )
	{
		i=f-src;
	}

	xs_pushNumber(vm,i);
	
	free(src);
	free(sub);
}

void string_atoi(_xsvm *vm)
{
	char *a=xs_popString(vm);
	int i=xs_atoi(a);
	
	xs_pushNumber(vm, i);
	free(a);
}
/*
void string_itoa(_xsvm *vm)
{
	int i=xs_popNumber(vm);
	
}

void string_printf(_xsvm *vm)
{
	return;
}
*/
void usestringlib(_xsvm *vm)
{
    xs_register(vm, "str_length",1, string_length);
    xs_register(vm, "str_toLower",1, string_toLower);
    xs_register(vm, "str_toUpper", 1,string_toUpper);
    xs_register(vm, "str_subString",3, string_subString);
    xs_register(vm, "str_indexOf", 2,string_indexOf);

}
