#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xslexer.h"
#include "xsvm.h"

char* Tokens1 = " ! ; + - * / % = ( ) { } [ ] < > , ";
char* Tokens2 = "&& || == >= <= != ++ -- ..";
enum TokenType
{
    TT_BINOP,
    TT_UNOP,
    TT_KEYWORDS,
    TT_INDENTIFER
};

struct Token
{           
    char* Symbol;
    int SrcLine;
    unsigned char Type;
};

void initLex(_xsvm *vm)
{
	if(!vm->lexer.script)
	{
		return;
	}

	vm->lexer.srcLen=strlen(vm->lexer.script);
	vm->lexer.currentLine=1;
	vm->lexer.tempTokenLen=0;
	vm->lexer.currentPos=vm->lexer.script;	
	memset(vm->lexer.currentToken,0,maxVariableLength);
	memset(vm->lexer.tempToken,0,maxVariableLength);
	//vm->lexer.varNameTable.count=0;
}

#define get_temp_token if(vm->lexer.tempToken[0]!=0){strcpy(vm->lexer.currentToken,vm->lexer.tempToken);memset(vm->lexer.tempToken,0,maxVariableLength);vm->lexer.tempTokenLen=0;return vm->lexer.currentToken;}

int ignoreComments(_xsvm *vm)
{
	int rt=0;
	if( vm->lexer.currentPos[0]=='/' && vm->lexer.currentPos[1]=='/')
	{
		rt++;
		while(vm->lexer.currentPos[0])
		{
			if(vm->lexer.currentPos[0]=='\n' )
			{
				vm->lexer.currentPos++;
				vm->lexer.currentLine++;
				break;
			}
			vm->lexer.currentPos++;
		}
	}
	if( rt>0)
	{
		xs_ignoreSpace(vm);
	}
	
	if( vm->lexer.currentPos[0]=='/' && vm->lexer.currentPos[1]=='*')
	{
		rt++;
		while(vm->lexer.currentPos[0] && vm->lexer.currentPos[1])
		{
			if(vm->lexer.currentPos[0]=='*' && vm->lexer.currentPos[1]=='/' )
			{
				vm->lexer.currentPos+=2;
				//vm->lexer.currentLine+=2;
				break;
			}
			if(vm->lexer.currentPos[0]=='\n' )
			{
				vm->lexer.currentLine++;
			}
			vm->lexer.currentPos++;
		}
	}
	
	if( rt>0)
	{
		xs_ignoreSpace(vm);
	}
		
	return rt;
}

int xs_isSpace(xsbyte b)
{
	return ( b==0x20 || b==0x09 || b=='\r' || b=='\n');
}

int xs_ignoreSpace(_xsvm *vm)
{
	int rt=0;
	while(xs_isSpace(vm->lexer.currentPos[0]))
	{
		rt++;
		if(vm->lexer.currentPos[0]=='\n')
		{
			vm->lexer.currentLine++;
		}

		vm->lexer.currentPos++;
		if(vm->lexer.currentPos[0]==0 )
		{
			break;
		}
	}
	if(	ignoreComments(vm)>0 )
	{
		rt=1;
	}
	
	return rt;
}


char* nextToken(_xsvm *vm)
{	
	if( vm->lexer.currentToken)
	{
		strcpy(vm->lexer.preToken,vm->lexer.currentToken);
	}

	if( vm->lexer.nextToken[0] )
	{
		strcpy(vm->lexer.currentToken,vm->lexer.nextToken);
		vm->lexer.nextToken[0]=0;
		return vm->lexer.currentToken;
	}
	
	memset(vm->lexer.currentToken,0,maxVariableLength);
	//为空格，或换行之类的，忽略
	//忽略注释	

	if( xs_ignoreSpace(vm)>0 )
	{
		get_temp_token
	}

	if(vm->lexer.currentPos[0]==0 )
	{
		return NULL;
	}
	strncpy(vm->lexer.currentToken,vm->lexer.currentPos,2);
	//双字节操作符
	if(!xs_isSpace(vm->lexer.currentToken[1]) && strstr(Tokens2,vm->lexer.currentToken) )
	{
		get_temp_token

		vm->lexer.currentPos+=2;
		return vm->lexer.currentToken;
	}
	//为单字节操作符
	else if( strchr(Tokens1,vm->lexer.currentPos[0] ) )
	{
		get_temp_token

		memset(vm->lexer.currentToken,0,maxVariableLength);
		strncpy(vm->lexer.currentToken,vm->lexer.currentPos,1);
		vm->lexer.currentPos++;
		return vm->lexer.currentToken;
	}
	else
	{
		memset(vm->lexer.currentToken,0,maxVariableLength);
		//为关键字或变量名，或数值、字符串，必须以空格或分号或操作符分开
		//string
		if(vm->lexer.currentPos[0]=='\"' )
		{
			char *p=NULL;
			for(;;)
			{
				p=strchr(vm->lexer.currentPos+1,'\"');
				if(!p)
				{
					break;
				}
				else
				{
					if( (p-1)[0]!='\\' || ((p-1)[0]=='\\'&&(p-2)[0]=='\\'))
					{
						break;
					}
				}
			}
			if(p)
			{
				get_temp_token

				strncpy(vm->lexer.currentToken,vm->lexer.currentPos,p+1-vm->lexer.currentPos);
				vm->lexer.currentPos=p+1;
				return vm->lexer.currentToken;
			}
		}
		else
		{
			//将单字节加入
			vm->lexer.tempToken[vm->lexer.tempTokenLen]=vm->lexer.currentPos[0];
			vm->lexer.tempTokenLen++;
			vm->lexer.tempToken[vm->lexer.tempTokenLen]=0;
			if( vm->lexer.tempTokenLen>=maxVariableLength)
			{
				//raise error
			}
			vm->lexer.currentPos++;
		}

	}

	return nextToken(vm);
}

