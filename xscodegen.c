
#include "xsdef.h"
#include "xscodegen.h"
#include "xsvm.h"
#include "xsoptimizer.h"
#include "xsconfig.h"


int xs_addCode(_xsvm *vm, int varAddr,xsbyte dataScope, xsbyte opCode)
{
	_stack *gc=&vm->gcode;
	_instruction ins;
	int count;
	if( vm->currentObject!=vm->globalObject )
	{
		gc=&vm->code;
	}
	count=gc->count;
	
	//if( vm->currentObject->code.count>MAX_FUNCTION_INSTRUCTION )
	if( vm->currentObject->codeCount>MAX_FUNCTION_INSTRUCTION )
	{
		xs_throw(VE_MAX_FUNCTION_INSTRUCTION);
	}
	ins.address=varAddr;
	ins.opCode=opCode;
	ins.scope=dataScope;
	
	//ins.layer=varLayer;
#ifdef XS_DEBUG
	ins.breakPoint=0;
	ins.lineNumber=vm->lexer.currentLine;
#endif
	
	//stackPush(&vm->currentObject->code,&ins);
	stackPush(gc,&ins);	
	xs_optimize(vm);
	
	if(vm->currentObject->codeCount==0 )
	{
		vm->currentObject->codeStart=count;
	}
	count=gc->count-count;
	vm->currentObject->codeCount+=count;

	return gc->count-1;
	//return vm->currentObject->code.count-1;
}

void xs_printEnv(_xsvm *vm)
{
	printf("\nStack: %d\t Const: %d\t Object: %d\n",vm->stackCount,vm->constVector.count,vm->staticObjects.count);
}

void xs_getLocalVariables(_xsvm *vm, char *dst)
{
	_object *o=vm->currentObject;
	char str[256];
	int j;
	char *n;
	//当前object的名字、和一些属性
	n=xs_getVarNameByAddr(vm, vm->currentObject->name);
	sprintf( str, "currentObject:%s\r\ncode:%d - %d\r\nvarCount:%d\r\n", n, vm->currentObject->codeStart, vm->currentObject->codeStart+vm->currentObject->codeCount, vm->currentObject->varCount);
	strcat( dst, str);
	
	for( j=0; j<o->varCount; j++ )
	{
		_variable *v=&o->variables[j];//stackIndex(&o->variables,j);
		n=xs_getVarNameByAddr(vm,v->name);
		if(n)
		{
			char *p=xs_toString(v);
			//
			if( strlen(p)>250 )
			{
				p[250]=0;
			}
			
			memset(str, 0, 256);	
			if(strstr(n,XSGO) )
			{
				sprintf(str, "%s=%s\r\n", n+3,p);					
				//printf("%s=%s\t",n+3,p);
			}
			else
			{
				sprintf(str, "%s=%s\r\n", n,p);
				//printf("%s=%s\t",n,p);
			}
			if(strlen(str)>0)
			{
				if(dst)
				{
					strcat(dst,str);
				}
				else
				{
					printf(str);
				}
			}

			free(p);				
		}
	}	
}

void xs_getGlobalVariables(_xsvm *vm, char *dst)
{
	_object *o=vm->globalObject;
	char str[256];

	int j;
	char *n;
	for( j=0; j<o->varCount; j++ )
	{
		_variable *v=&o->variables[j];//stackIndex(&o->variables,j);
		n=xs_getVarNameByAddr(vm,v->name);
		if(n)
		{
			char *p=xs_toString(v);
			//
			if( strlen(p)>250 )
			{
				p[250]=0;
			}
				
			memset(str, 0, 256);
			if(strstr(n,XSGO) )
			{
				sprintf(str, "%s=%s\r\n", n+3,p);					
				//printf("%s=%s\t",n+3,p);
			}
			else
			{
				sprintf(str, "%s=%s\r\n", n,p);
				//printf("%s=%s\t",n,p);
			}
			if(strlen(str)>0)
			{
				if(dst)
				{
					strcat(dst,str);
				}
				else
				{
					printf(str);
				}
			}

			free(p);				
		}
	}	
}

void xs_printVariables(_xsvm *vm, char *dst)
{
	int i;
	int j;
	char *n;
	
	char str[256];
	for(i=0;i<vm->staticObjects.count;i++)
	{
		_object *o=(_object*)stackIndex(&vm->staticObjects,i);// listGet(&vm->objectVector,i);
		n=xs_getVarNameByAddr(vm, o->name);
				
		memset(str, 0, 256);
		if(n)
		{			
			if( o->type==dtFunction )
			{
				sprintf(str, "\nFUNC %s:\t",n);
			}
			else if(o->type==dtClass )
			{
				if( o->typeName==o->name )
				{
					sprintf(str, "\nCLASS %s:\t",n);
				}
				else
				{
					char *tn=xs_getVarNameByAddr(vm,o->typeName);
					sprintf(str, "\nOBJECT %s [%s]:\t",n,tn);
				}
			}			
		}

		if(strlen(str)>0)
		{
			if(dst)
			{
				strcat(dst,str);
			}
			else
			{
				printf(str);
			}
		}
		
		
		for( j=0; j<o->varCount; j++ )
		{
			_variable *v=&o->variables[j];//stackIndex(&o->variables,j);
			n=xs_getVarNameByAddr(vm,v->name);
			if(n)
			{
				
				char *p=xs_toString(v);
				//
				if( strlen(p)>250 )
				{
					p[250]=0;
				}
				memset(str,0,256);	
				if(strstr(n,XSGO) )
				{
					sprintf(str, "%s=%s\r\n", n+3,p);					
					//printf("%s=%s\t",n+3,p);
				}
				else
				{
					sprintf(str, "%s=%s\r\n", n,p);
					//printf("%s=%s\t",n,p);
				}
				if(strlen(str)>0)
				{
					if(dst)
					{
						strcat(dst,str);
					}
					else
					{
						printf(str);
					}
				}

				free(p);				
			}
		}

	}

}

//反编译指令码
void xs_getInsString(_xsvm *vm, _object *o, _instruction *ins, char *buf)
{
	char *op=xs_getOpString(ins->opCode);
	_object *oo=vm->currentObject;

	_variable *v;
	_variable vr;
	//_variable *v2=NULL;

	vm->currentObject=o;
/*
	if( ins->scope==XSCONST)
	{
		v=stackIndex(&vm->constVector,ins->address);
	}
	else
	{
*/
	if( ins->opCode==opCALL )
	{
		_object *co=(_object*)stackIndex(&vm->staticObjects,ins->address);//listGet(&vm->objectVector,ins->address);
		v=(_variable*)stackIndex(&vm->lexer.varNameTable,co->name);
	}
	else if((ins->opCode>=opCONC && ins->opCode<=opLOOPEND) || ins->opCode==opNOP || ins->opCode==opRESSTACK)
	{
		_object *co=(_object*)stackIndex(&vm->staticObjects,0);//listGet(&vm->objectVector,0);
		v=(_variable*)stackIndex(&vm->lexer.varNameTable,co->name);
	}
	else if(ins->opCode>=opJMP && ins->opCode<=opJNE )
	{
		vr.type=dtNumber;
		vr.v.Number=ins->address;
		v=&vr;
	}
	else if(ins->opCode==opCALLAPI)
	{
		_xsapi *api=(_xsapi *)vm->apiStack.buff+ins->address;
		vr.type=dtString;
		vr.v.String=api->apiName;
		v=&vr;
	}
	else
	{
		//取得变量值

		DEFVARS
		v=&VAR(ins->scope,ins->address);
		
		if( ins->scope!=XSCONST) v=(_variable*)vm->lexer.varNameTable.buff+v->name;

		
	}
	//}
	
	if(v->type==dtString)
	{
		//if( strcmp(v->v.String,XSGO)!=0 )
		if(strlen(v->v.String)>strlen(XSGO) && strstr(v->v.String,XSGO) )
		{
			sprintf(buf,"%s\t\t%s \n",op, v->v.String+3);
		}
		else
		{
			sprintf(buf,"%s\t\t\n",op);
		}
	}
	else
	{
		char *p=xs_toString(v);
		sprintf(buf,"%s\t\t%s\n",op,p);
		free(p);				
		//sprintf(buf,"%s\t\t%f\n",op,v->v.Number);
	}
	vm->currentObject=oo;
}
#define maxInsString	512

int xs_deCompileToFile(_xsvm *vm, char *filename)
{
	FILE *fh;
	int j;
	char buf[maxInsString];
	fh=fopen(filename,"w");
	if(fh==NULL )
	{
		//printf("Can not open file: %s, press Enter to exit...",xsfile);
		//getchar();
		return 0;
	}

	for( j=0; j<vm->code.count; j++ )
	{
		char b[maxVariableLength];
		_instruction *ins=(_instruction*)stackIndex(&vm->code,j);
		_object *o=NULL;
		int i;
		for(i=0;i<vm->staticObjects.count;i++)
		{
			o=(_object*)stackIndex(&vm->staticObjects,i);
			if( j>=o->codeStart && j<(o->codeStart+o->codeCount) )
			{
				break;
			}
		}
		
		memset(buf,0,maxInsString);			

		memset(b,0,maxVariableLength);
		xs_getInsString(vm,o, ins,b);
		sprintf(buf,"%d\t %s",j,b);

		fwrite(buf,1,strlen(buf),fh);

	}

	fclose(fh);
	return XS_SUCCESS;

}

#define PRINT printf(buf)
//#define PRINT strcat(dst,buf)
int xs_deCompile(_xsvm *vm, char* dst)
{
	int j;
	char buf[maxInsString];
	
	for( j=0; j<vm->code.count; j++ )
	{
		char b[64];
		_instruction *ins=(_instruction*)stackIndex(&vm->code,j);

		_object *o=NULL;
		int i;
		for(i=0;i<vm->staticObjects.count;i++)
		{
			o=(_object*)stackIndex(&vm->staticObjects,i);
			if( j>=o->codeStart && j<(o->codeStart+o->codeCount) )
			{
				break;
			}
		}
		
		memset(buf,0,maxInsString);			

		memset(b,0,64);
		xs_getInsString(vm,o, ins,b);
		sprintf(buf,"%d\t %s",j,b);
		//PRINT;
		if(dst)
		{
			strcat(dst,buf);
		}
		else
		{
			printf(buf);
		}
	}

	return XS_SUCCESS;
}
char* xs_getOpString(xsbyte op)
{
    switch (op)
    {
		case opNOP:		return "NOP";

		case opCONC:	return	"CONC";
        case opADD:		return "ADD";
        case opSUB:		return "SUB";
        case opMUL:		return "MUL";
        case opDIV:		return  "DIV";
		case opMOD:		return "MOD";
        case opINC:     return "INC";
        case opDEC:     return "DEC";

        case opEQ:		return  "EQ";
        case opGE:		return  "GE";
        case opGT:		return  "GT";
        case opLE:		return  "LE";
        case opLT:		return  "LT";
		case opNE:		return  "NE";

		case opNOT:		return "NOT";
		case opNEG:		return	"NEG";
		case opAND:		return  "AND";
		case opOR:		return  "OR";

		case opASSIGN:	return	"ASSIGN";
		case opPUSH:	return	"PUSH";
		case opPOP:		return	"POP";

		case opCALL:	return	"CALL";
		case opJMP:		return	"JMP";
        case opJNZ:     return  "JNZ";
        case opJG:      return  "JG";       //1 大于>greater than
        case opJGE:     return  "JGE";  //2 大于等于>=greater or equal to
        case opJL:      return  "JL"; //3 小于<
        case opJLE:     return  "JLE";  //4 小于等于<=less or equal to
        case opJEQ:     return  "JEQ";  //5 相等equal
        case opJNE:		return  "JNE";//6	不相等

		case opTEST:	return	"TEST";
		case opRET:		return	"RET";

		case opNEW:		return	"NEW";
		case opLOOP:	return	"LOOP";	//loop begin
		case opLOOPEND:	return	"LOOPEND";	//loop end

		case opCALLAPI:	return	"CALLAPI";
		case opRESSTACK:return	"RESSTACK";
		
		case opPUSHARR:	return	"PUSHARR";
		case opASSIGNARR:return	"ASSIGNARR";
    }

	
	return "ERR";
}
