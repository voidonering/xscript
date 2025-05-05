/*
2009/03/01 TangJun
xs vm core
*/

#include "xsvm.h"
#include "xsiolib.h"
#include "xsmathlib.h"
#include "xssystemlib.h"
#include "xsqueuelib.h"
#include "xsdef.h"
#include "xs.h"
#include "xsgc.h"
#include "xslog.h"
#include "xsutil.h"

//-----------------------------------------------------------------------------------
_xsvm* xs_init(char *workingDirectory)
{
	int go;
	_xsvm *vm=(_xsvm*)calloc(sizeof(_xsvm),1);
	strcpy(vm->workingPath, workingDirectory);
	
	stackNew(&vm->lexer.varNameTable,sizeof(_variable),64);
	stackNew(&vm->constVector,sizeof(_variable),64);
	stackNew(&vm->staticObjects,sizeof(_object),64);
	stackNew(&vm->code, sizeof(_instruction), 256);
	go=xs_addObject(vm,XSGO,dtFunction);
	vm->globalObject=(_object*)stackIndex(&vm->staticObjects,go);

	vm->currentObject=vm->globalObject;
	stackNew(&vm->apiStack,sizeof(_xsapi),32);
	vm->gch=xs_createGc();
	xs_loadLibs(vm);
	
	return vm;
}

//-----------------------------------------------------------------------------------
void xs_loadLibs(_xsvm *vm)
{
	xs_try
	{
		useiolib(vm);
		usemathlib(vm);
		usesystemlib(vm);
		usequeuelib(vm);
	}
	xs_catch
	{
		return;
	}
}

//-----------------------------------------------------------------------------------
void xs_free(_xsvm *vm )
{
	int i,j;
	_variable *v;
	_object *o;
	
	for(i=0; i<vm->lexer.varNameTable.count; i++)
	{
		v=(_variable*)stackIndex(&vm->lexer.varNameTable,i);
		xs_freeVariable(vm,v);
	}
	
	stackFree(&vm->lexer.varNameTable);
	//free consts
	for(i=0; i<vm->constVector.count; i++)
	{
		v=(_variable*)stackIndex(&vm->constVector,i);
		xs_freeVariable(vm,v);
	}
	stackFree(&vm->constVector);

	stackFree(&vm->apiStack);

	//free objects
	for(i=0; i<vm->staticObjects.count; i++)
	{
		o=(_object*)stackIndex(&vm->staticObjects,i);
		for(j=0; j<o->varCount; j++ )
		{
			xs_freeVariable(vm,&o->variables[j]);
		}

	}
	stackFree(&vm->staticObjects);
	hashtable_destroy(vm->gch,0);
	
	stackFree(&vm->code);
	free(vm->lexer.script);
	free(vm);
}

//-----------------------------------------------------------------------------------
int xs_compile(_xsvm *vm,  char* script)
{
	//lex
	int i;
	int rt=XS_SUCCESS;
	char *newscript=(char*)calloc(strlen(script)+10, 1);
	strcpy(newscript,script);
	vm->lexer.script=newscript;
	initLex(vm);
	stackNew( &vm->gcode, sizeof(_instruction), 256);
	xs_try
	{
		rt=parseStatements(vm);
	}
	xs_catch
	{
		stackFree( &vm->gcode);
		return XS_EXCEPTION;
	}

	vm->globalObject->codeStart=vm->code.count;
	vm->globalObject->codeCount=vm->gcode.count;
	for( i=0; i<vm->gcode.count; i++ )
	{
		_instruction *ins=(_instruction*)stackIndex(&vm->gcode, i);
		//检查所有的跳转,加上偏移值
		
		switch( ins->opCode)
		{
			case opJG:
			case opJGE:
			case opJL:
			case opJLE:
			case opJEQ:
			case opJNE:
			case opJNZ:
			case opJMP: 
			{
				ins->address+=vm->globalObject->codeStart;
				break;
			}
			
		}
		
		stackPush( &vm->code, ins);
	}
	
	stackFree( &vm->gcode);
	return rt;
}

//-----------------------------------------------------------------------------------
int xs_run( _xsvm *vm)
{
	//从global开始执行，也即staticObjects的第一个元素的代码
	if(vm->globalObject)
	{
		//设置当前对象
		vm->currentObject=vm->globalObject;
		xs_try
		{
			//xs_steps(vm);
			//脚本启动后挂起线程,等待命令
#ifdef XS_DEBUG
			vm->debugger.suspend(vm);
			vm->debugger.isBusy=1;
			//((xsDebugger*)vm->debugger.debugger)->setBusyMode(1);
#endif
			xs_callFunction(vm, vm->currentObject); 
#ifdef XS_DEBUG			
			//((xsDebugger*)vm->debugger.debugger)->setBusyMode(0);
			vm->debugger.isBusy=0;
#endif
		}
		xs_catch
		{
			return XS_EXCEPTION;
		}
		return XS_SUCCESS;
	}
	return XS_EXCEPTION;
}

void xs_callFunction(_xsvm *vm, _object *o) 												
{																		
	//对于递归调用，需要为每个函数保存一个内部变量表								
	int i, pcount;																
	_object oi;															
	_object *oo=vm->currentObject;		
	//int stackCount=0;
	//如果不是GApplication本身，或不是递归调用，则不创建实例					
	if(o->typeName==oo->typeName && o->parent )												
	{																	
		vm->currentObject=&oi;											
		oi=*o;						
	}																									
	else																								
	{																										
		vm->currentObject=o;																				
	}																										
	//参数赋值																						
	pcount=vm->currentObject->paramCount;	
	i=pcount;
	while( i>0 )																						
	{		
		i--;
		if(vm->stackCount<1)
		{
			xs_throw(vm->lastError);
		}
		vm->stackCount--;
		vm->currentObject->variables[i].v=vm->globalStack[vm->stackCount].v;
		vm->currentObject->variables[i].type=vm->globalStack[vm->stackCount].type;
	}
	
	//stackCount=vm->stackCount;
	if( xs_steps(vm)==XS_EXCEPTION )
	{
		xs_throw(vm->lastError);
	}
	//
	i=pcount;
	while( i>0 )																						
	{		
		i--;
		xs_freeVariable(vm,&vm->currentObject->variables[i]);
	}
	vm->currentObject=oo;
}


//-----------------------------------------------------------------------------------
void xs_freeArray(_xsvm *vm, _variable *v)
{
	_stack *s=(_stack*)(unsigned int)v->v.Number;
	_variable *var=(_variable*)s->buff;
	int k;
	for(k=0; k<s->count; k++)
	{
		if(var[k].type==dtObject)
		{
			xs_removeRef(vm, var[k].v.Number);
		}
		else if(var[k].type==dtString)
		{
			free(var[k].v.String);
		}
	}
	stackFree(s);
	free(s);	
}

//-----------------------------------------------------------------------------------
void xs_freeVariable(_xsvm *vm,_variable *v)
{
	if(v->type==dtObject)
	{
		if(v->v.Number!=0)
		{
			xs_removeRef(vm, v->v.Number);
		}
	}
	else if(v->type==dtString)
	{
		if(v->v.String)
		{
			free(v->v.String);
		}
	}
	else if(v->type==dtArray)
	{
		if(v->v.Number!=0)
		{
			xs_freeArray(vm,v);
		}
	}
	v->type=dtNull;
}
//-----------------------------------------------------------------------------------
__inline void xs_assign(_xsvm *vm, _variable *vl,_variable *vr)	
{
	xs_freeVariable(vm,vl);												
	vl->type=vr->type;													
	vl->v=vr->v;
	if(vl->type==dtObject)
	{
		xs_addRef(vm,vl->v.Number);
	}
	else if(vl->type==dtString)												
	{																	
		vl->v.String=(char*)malloc(strlen(vr->v.String)+1);					
		strcpy(vl->v.String,vr->v.String);	
	}																	
	else if(vl->type==dtArray)												
	{			
		int i;
		_variable *v;
		_stack *s=(_stack *)calloc(1,sizeof(_stack));								
		_stack *sr=(_stack*)(unsigned int)vr->v.Number;									
		stackNew(s,sizeof(_variable),sr->count);						
		vl->v.Number=(unsigned int)s;												
		stackCopy(s,sr);

		v=(_variable*)s->buff;
		for(i=0; i<s->count; i++ )
		{
			if(v[i].type==dtObject)
			{
				xs_addRef(vm, v[i].v.Number);
			}
		}
	}																	
}

//-----------------------------------------------------------------------------------
void xs_onScriptUpdate(_xsvm *vm)
{
	if(vm->onUpdate)
	{
		xs_try
		{
#ifdef XS_DEBUG
			vm->debugger.isBusy=1;
#endif
			xs_callFunction(vm, vm->onUpdate);
#ifdef XS_DEBUG
			vm->debugger.isBusy=0;
#endif
		}
		xs_catch
		{
			//
		}
	}
}
//由调用者将参数压入栈,可以自由压入数字、字串、对象等
//-----------------------------------------------------------------------------------
xsbyte xs_onScriptEvent(_xsvm *vm )//, XS_NUMBER Sender,XS_NUMBER eventID, XS_NUMBER e)
{
	if(vm->onEvent)
	{
		xs_try
		{
#ifdef XS_DEBUG
			vm->debugger.isBusy=1;
#endif
			vm->currentObject=vm->globalObject;
			xs_callFunction(vm, vm->onEvent);
			
			return xs_popNumber(vm);
#ifdef XS_DEBUG
			vm->debugger.isBusy=0;
#endif
		}
		xs_catch
		{
			//
		}
	}
	else
	{
		//将push到栈内的值全部pop出
		xs_popNumber(vm);	
		xs_popNumber(vm);
		xs_popNumber(vm);
		
		return 0;
	}
	
	return 0;
}

int xs_steps(_xsvm *vm )
{	
	register _instruction *beg=(_instruction *)vm->code.buff;
	register _instruction *ins=beg-1+vm->currentObject->codeStart;
	register _instruction *end=beg+vm->currentObject->codeStart+vm->currentObject->codeCount;
	register _variable *vl;
	register _variable *vr;
	
	//09.10.09
	//int stackCount=vm->stackCount;
	
	DEFVARS
	
	for(;;)
	{
		ins++;
BEGIN:
		if(ins>=end) return XS_SUCCESS;
#ifdef XS_DEBUG
		//send current position, source line and instruction point
		vm->debugger.sendPosition(vm, ins->lineNumber, ins-beg+1 );

		if( ins->breakPoint )
		{
			vm->debugger.stepmode=1;
			vm->debugger.paused=1;
		}
		
		if(vm->debugger.paused )
		{
			//pause for thread
			vm->debugger.suspend(vm);
		}
		
		if( vm->debugger.stepmode)
		{
			vm->debugger.paused=1;
		}
#endif
		switch(ins->opCode)
		{
			case opPUSH:
			{
				if(vm->stackCount>=maxStackCount) xs_throw(VE_STACK_OVERFLOW);
				vm->globalStack[vm->stackCount].type=dtNull;
				xs_assign(vm, &vm->globalStack[vm->stackCount],&VAR(ins->scope,ins->address));
				vm->stackCount++;
				continue;
			}
			case opPOP:
			{
				vm->stackCount--;
				xs_freeVariable(vm, &vm->globalStack[vm->stackCount]);
				continue;
			}
			case opNEG:
			{
				if(ins->scope==XSSTACK)
					vm->globalStack[vm->stackCount-1].v.Number=-vm->globalStack[vm->stackCount-1].v.Number;
				else
				{
					vm->globalStack[vm->stackCount].type=dtNumber;
					vm->globalStack[vm->stackCount].v.Number=-VAR(ins->scope,ins->address).v.Number;
					vm->stackCount++;
				}

				continue;
			}
			case opCONC:
			{
				//连接2个字串
				char *p;
				int len1,len2;
				char *buf;
				if(vm->stackCount<2 ) xs_throw(VE_STACK_ERROR);
				vm->stackCount--;
				vr=vm->globalStack+vm->stackCount;
				vl=vm->globalStack+vm->stackCount-1;
				//复制字符串
				if(vl->type!=dtString)
				{
					xs_convertToString(vm,vl);
				}

				p=xs_toString(vr);
				len1=strlen(p);
				len2=strlen(vl->v.String);
				buf=(char*)calloc(len1+len2+1,1);
				strcpy(buf,vl->v.String);
				strcat(buf,p);
				free(p);
				free(vl->v.String);
				vl->v.String=buf;	
				
				xs_freeVariable(vm,vr);
				continue;
			}
			case opADD:
			{
				//if(vm->stackCount<2 ) xs_throw(VE_STACK_ERROR);
				if(ins->scope2!=XSSTACK)
				{
					vm->globalStack[vm->stackCount].type=dtNumber;
					vm->globalStack[vm->stackCount].v.Number=VAR(ins->scope,ins->address).v.Number+VAR(ins->scope2,ins->address2).v.Number;
					vm->stackCount++;
				}
				else if(ins->scope!=XSSTACK)
				{
					vm->globalStack[vm->stackCount-1].v.Number+=VAR(ins->scope,ins->address).v.Number;
				}
				else
				{
					vm->stackCount--;
					vm->globalStack[vm->stackCount-1].v.Number+=vm->globalStack[vm->stackCount].v.Number;
				}
				continue;
			}
			case opSUB:
			{
				if(ins->scope2!=XSSTACK)
				{
					vm->globalStack[vm->stackCount].type=dtNumber;
					vm->globalStack[vm->stackCount].v.Number=VAR(ins->scope2,ins->address2).v.Number-VAR(ins->scope,ins->address).v.Number;
					vm->stackCount++;
				}
				else if(ins->scope!=XSSTACK)
				{
					vm->globalStack[vm->stackCount-1].v.Number-=VAR(ins->scope,ins->address).v.Number;
				}
				else
				{
					vm->stackCount--;
					vm->globalStack[vm->stackCount-1].v.Number-=vm->globalStack[vm->stackCount].v.Number;
				}
				continue;
			}
			case opINC:
			{
				VAR(ins->scope,ins->address).v.Number++;//(_variable *)oo->variables.buff+ins->address;                
				continue;
			}
			case opDEC:
			{
				VAR(ins->scope,ins->address).v.Number--;//(_variable *)oo->variables.buff+ins->address;                
				continue;
			}
			case opMUL:
			{		
				if(ins->scope2!=XSSTACK)
				{
					vm->globalStack[vm->stackCount].type=dtNumber;
					vm->globalStack[vm->stackCount].v.Number=VAR(ins->scope2,ins->address2).v.Number*VAR(ins->scope,ins->address).v.Number;
					vm->stackCount++;
				}
				else if(ins->scope!=XSSTACK)
				{
					vm->globalStack[vm->stackCount-1].v.Number*=VAR(ins->scope,ins->address).v.Number;
				}
				else
				{
					vm->stackCount--;
					vm->globalStack[vm->stackCount-1].v.Number*=vm->globalStack[vm->stackCount].v.Number;
				}
				continue;
			}
			case opDIV:
			{	
				if(ins->scope2!=XSSTACK)
				{
					vm->globalStack[vm->stackCount].type=dtNumber;
					vm->globalStack[vm->stackCount].v.Number=VAR(ins->scope2,ins->address2).v.Number/VAR(ins->scope,ins->address).v.Number;
					vm->stackCount++;
				}
				else if(ins->scope!=XSSTACK)
				{
					vm->globalStack[vm->stackCount-1].v.Number=vm->globalStack[vm->stackCount-1].v.Number/VAR(ins->scope,ins->address).v.Number;
				}
				else
				{
					vm->stackCount--;
					vm->globalStack[vm->stackCount-1].v.Number=vm->globalStack[vm->stackCount-1].v.Number/vm->globalStack[vm->stackCount].v.Number;
				}
				continue;
			}
			case opMOD:
			{	
				if(ins->scope2!=XSSTACK)
				{
					vm->globalStack[vm->stackCount].type=dtNumber;
					vl=&VAR(ins->scope2,ins->address2);
					vr=&VAR(ins->scope,ins->address);
					vm->globalStack[vm->stackCount].v.Number=vl->v.Number - ((int)(vl->v.Number/vr->v.Number))*vr->v.Number;

					vm->stackCount++;
				}
				else if(ins->scope!=XSSTACK)
				{
					vl=&vm->globalStack[vm->stackCount-1];
					vr=&VAR(ins->scope,ins->address);
					vm->globalStack[vm->stackCount-1].v.Number=vl->v.Number - ((int)(vl->v.Number/vr->v.Number))*vr->v.Number;
				}
				else
				{
					vm->stackCount--;
					vl=&vm->globalStack[vm->stackCount-1];
					vr=&vm->globalStack[vm->stackCount];
					vm->globalStack[vm->stackCount-1].v.Number=vl->v.Number - ((int)(vl->v.Number/vr->v.Number))*vr->v.Number;
				}
				continue;
			}
			case opASSIGN:
			{
				if(ins->scope2==XSSTACK)
				{
					vm->stackCount--;
 					vr=vm->globalStack+vm->stackCount;

					vl=&VAR(ins->scope,ins->address);
					
					//复制字串
					xs_assign(vm,vl,vr);
					xs_freeVariable(vm,vr);				
				}
				else
				{
					//vm->globalStack[vm->stackCount].v=VAR(ins->scope,ins->address).v;
					vl=&VAR(ins->scope,ins->address);
					vr=&VAR(ins->scope2,ins->address2);
					xs_assign(vm,vl,vr);
				}
				continue;
			}
			case opGT:
			{
				if(vm->stackCount<2) xs_throw(VE_STACK_ERROR);
				vm->stackCount--;

                vm->globalStack[vm->stackCount-1].v.Number=(vm->globalStack[vm->stackCount-1].v.Number>vm->globalStack[vm->stackCount].v.Number);
				continue;
			}
			case opGE:
			{
				if(vm->stackCount<2) xs_throw(VE_STACK_ERROR);
				vm->stackCount--;

                vm->globalStack[vm->stackCount-1].v.Number=(vm->globalStack[vm->stackCount-1].v.Number>=vm->globalStack[vm->stackCount].v.Number);
				continue;
			}
			case opLT:
			{
				if(vm->stackCount<2) xs_throw(VE_STACK_ERROR);
				vm->stackCount--;

                vm->globalStack[vm->stackCount-1].v.Number=(vm->globalStack[vm->stackCount-1].v.Number<vm->globalStack[vm->stackCount].v.Number);
				continue;
			}
			case opLE:
			{
				if(vm->stackCount<2) xs_throw(VE_STACK_ERROR);
				vm->stackCount--;

                vm->globalStack[vm->stackCount-1].v.Number=(vm->globalStack[vm->stackCount-1].v.Number<=vm->globalStack[vm->stackCount].v.Number);
				continue;
			}
			case opEQ:
			{
				XS_NUMBER e;
				if(vm->stackCount<2) xs_throw(VE_STACK_ERROR);
				vm->stackCount--;
				
				if( vm->globalStack[vm->stackCount-1].type==vm->globalStack[vm->stackCount].type )
				{
					if( vm->globalStack[vm->stackCount-1].type==dtNumber)
					{
						e=(vm->globalStack[vm->stackCount-1].v.Number==vm->globalStack[vm->stackCount].v.Number);
					}
					else if( vm->globalStack[vm->stackCount-1].type==dtString)
					{
						e=(strcmp(vm->globalStack[vm->stackCount-1].v.String,vm->globalStack[vm->stackCount].v.String)==0);
					}
					else
					{
						e=0;
					}
				}
				else
				{
					e=0;
				}

				xs_freeVariable(vm,&vm->globalStack[vm->stackCount-1]);
				xs_freeVariable(vm,&vm->globalStack[vm->stackCount]);
				
				vm->globalStack[vm->stackCount-1].v.Number=e;
				continue;
			}
			case opNE:
			{
				XS_NUMBER e;
				if(vm->stackCount<2) xs_throw(VE_STACK_ERROR);
				vm->stackCount--;
				e=(vm->globalStack[vm->stackCount-1].v.Number!=vm->globalStack[vm->stackCount].v.Number);
				xs_freeVariable(vm,&vm->globalStack[vm->stackCount-1]);
				xs_freeVariable(vm,&vm->globalStack[vm->stackCount]);
								
				vm->globalStack[vm->stackCount-1].v.Number=e;
                
                continue;
			}
			case opJG:
			{
				if(vm->stackCount<2) xs_throw(VE_STACK_ERROR);
				vm->stackCount-=2;
				if(vm->globalStack[vm->stackCount].v.Number<=vm->globalStack[vm->stackCount+1].v.Number )
				{
					ins=beg+ins->address;
                    goto BEGIN;
				}
				continue;
			}
			case opJGE:
			{
				if(vm->stackCount<2) xs_throw(VE_STACK_ERROR);
				vm->stackCount-=2;
				if(vm->globalStack[vm->stackCount].v.Number<vm->globalStack[vm->stackCount+1].v.Number )
				{
					ins=beg+ins->address;
                    goto BEGIN;
				}
				continue;
			}
			case opJL:
			{
				if(vm->stackCount<2) xs_throw(VE_STACK_ERROR);
				vm->stackCount-=2;
				if(vm->globalStack[vm->stackCount].v.Number>=vm->globalStack[vm->stackCount+1].v.Number )
				{
					ins=beg+ins->address;
                    goto BEGIN;
				}
				continue;
			}
			case opJLE:
			{
				if(vm->stackCount<2) xs_throw(VE_STACK_ERROR);
				vm->stackCount-=2;
				if(vm->globalStack[vm->stackCount].v.Number>vm->globalStack[vm->stackCount+1].v.Number )
				{
					ins=beg+ins->address;
                    goto BEGIN;
				}
				continue;
			}
			case opJEQ:
			{
				XS_NUMBER e;
				if(vm->stackCount<2) xs_throw(VE_STACK_ERROR);
				vm->stackCount-=2;
				e=(vm->globalStack[vm->stackCount].v.Number!=vm->globalStack[vm->stackCount+1].v.Number );
				xs_freeVariable(vm,&vm->globalStack[vm->stackCount+1]);
				xs_freeVariable(vm,&vm->globalStack[vm->stackCount]);
									
				if(e)	
				{
					ins=beg+ins->address;
                    goto BEGIN;
				}
				continue;
			}
			case opJNE:
			{
				XS_NUMBER e;
				if(vm->stackCount<2) xs_throw(VE_STACK_ERROR);
				vm->stackCount-=2;
				e=(vm->globalStack[vm->stackCount].v.Number==vm->globalStack[vm->stackCount+1].v.Number );
				xs_freeVariable(vm,&vm->globalStack[vm->stackCount+1]);
				xs_freeVariable(vm,&vm->globalStack[vm->stackCount]);
									
				if(e)	
				{
					ins=beg+ins->address;
                    goto BEGIN;
				}
				continue;
			}
			case opNOT:
			{
				XS_NUMBER e;
				if(vm->stackCount<1) xs_throw(VE_STACK_ERROR);
                e=(vm->globalStack[vm->stackCount-1].v.Number==0);
                xs_freeVariable(vm,&vm->globalStack[vm->stackCount-1]);
                vm->globalStack[vm->stackCount-1].v.Number=e;
				continue;
			}
			case opAND:
			{
				XS_NUMBER e;
				if(vm->stackCount<2) xs_throw(VE_STACK_ERROR);
				vm->stackCount--;
				e=(vm->globalStack[vm->stackCount-1].v.Number&&vm->globalStack[vm->stackCount].v.Number);
				xs_freeVariable(vm,&vm->globalStack[vm->stackCount-1]);
				xs_freeVariable(vm,&vm->globalStack[vm->stackCount]);
				
                vm->globalStack[vm->stackCount-1].v.Number=e;
				continue;
			}
			case opOR:
			{
				XS_NUMBER e;
				if(vm->stackCount<2) xs_throw(VE_STACK_ERROR);
				vm->stackCount--;
				e=(vm->globalStack[vm->stackCount-1].v.Number||vm->globalStack[vm->stackCount].v.Number);
				xs_freeVariable(vm,&vm->globalStack[vm->stackCount-1]);
				xs_freeVariable(vm,&vm->globalStack[vm->stackCount]);
				
                vm->globalStack[vm->stackCount-1].v.Number=e;
				continue;
			}

			//function call
			case opCALL:
			{
				//对于递归调用，需要为每个函数保存一个内部变量表
                _object *o=(_object*)stackIndex(&vm->staticObjects,ins->address);
                //09.10.09
				//stackCount=vm->stackCount;
				
				xs_callFunction(vm, o); 
				continue;
			}
			case opRESSTACK:
			{
				//09.10.09
				//vm->stackCount=stackCount;
				continue;
			}
			case opCALLAPI:
			{				
				_xsapi *api=(_xsapi *)vm->apiStack.buff+ins->address;
				//09.10.09
				//stackCount=vm->stackCount;
				
				api->api(vm);
				continue;
			}
			//new class object
			case opNEW:
			{
				if( xs_newObject(vm,ins->address)==XS_EXCEPTION )
				{
					return XS_EXCEPTION;
				}
				continue;
			}
			case opRET:
			{
				return XS_SUCCESS;
			}
            case opJNZ:
            {
				if(vm->stackCount<1) xs_throw(VE_STACK_ERROR);
				vm->stackCount--;
				vr=vm->globalStack+vm->stackCount;

				if(vr->v.Number==0 )
				{
					ins=beg+ins->address;
                    goto BEGIN;
				}
				continue;

            }
			case opTEST:
			{
				if(vm->stackCount<1) xs_throw(VE_STACK_ERROR);
				vm->stackCount--;
				vr=vm->globalStack+vm->stackCount;

				if(vr->v.Number!=0 )
				{
					ins++;
				}
				continue;
			}
			case opJMP:
			{
				ins=beg+ins->address;
				goto BEGIN;
			}
			case opPUSHARR:
			{
				vl=&VAR(ins->scope,ins->address);

				if(vl->type!=dtArray)
				{
					xs_throw(VE_INVALID_ARRAY);
				}
				
				if(vm->stackCount<1) xs_throw(VE_STACK_ERROR);
				vr=&vm->globalStack[vm->stackCount-1];
				
				getArrayValue(vm,vl,vr->v.Number,vr);
				continue;
			}
			case opASSIGNARR:
			{
				vl=&VAR(ins->scope,ins->address);
				if(vm->stackCount<2) xs_throw(VE_STACK_ERROR);
				vm->stackCount-=2;
				vr=&vm->globalStack[vm->stackCount+1];
				setArrayValue(vm,vl,vr->v.Number,&vm->globalStack[vm->stackCount]);
				xs_freeVariable(vm,&vm->globalStack[vm->stackCount]);
				
				continue;
			}
		}
		//ins++;
	}

}

void getArrayValue(_xsvm *vm,_variable *vl,XS_NUMBER index, _variable *vr )
{
	_stack *s=(_stack*)(unsigned int)vl->v.Number;
	if(index>=0 && index<s->count)
	{
		_variable *v=(_variable *)stackIndex(s,index);
		xs_assign(vm,vr,v);
		//*vr=*v;
	}
}

void setArrayValue(_xsvm *vm,_variable *vl,XS_NUMBER index, _variable *vr)
{
	_stack *s;
	_variable *v;
	int i;
	_variable tv;
	tv.type=dtNull;
	if( index<0 )
	{
		return;
	}
	//如果不是数组，转化为数组
	if(vl->type!=dtArray)
	{
		vl->type=dtArray;
		s=(_stack *)calloc(1,sizeof(_stack));
		stackNew(s,sizeof(_variable),index);
		vl->v.Number=(unsigned int)s;
	}
	else
	{
		s=(_stack*)(unsigned int)vl->v.Number;
	}
	
	for(i=s->count; i<=index; i++)
	{
		stackPush( s, &tv);
	}
	
//	if(index>=s->count)
//	{
//		s->count=index+2;
//		stackResize(s, index+2);
//	}
	
	v=(_variable *)stackIndex(s,index);
	//防止被释放掉,设置为number
	//v->type=dtNumber;
//	if(v==NULL)
//	{																						{
//		return;
//	}
	xs_assign(vm,v,vr);
	
}
//-----------------------------------------------------------------------------------
int xs_addConst(_xsvm *vm,  _variable d)
{
	//已存在，则直接返回地址
	int i;
	for(i=0; i<vm->constVector.count;i++)
	{
		_variable *dt=(_variable*)stackIndex(&vm->constVector,i);
		if(dt->type==d.type )
		{
			if( (d.type==dtString && strcmp(d.v.String,dt->v.String)==0)||
				(d.type==dtObject && d.v.Number==dt->v.Number)||
				(d.type==dtNumber && d.v.Number==dt->v.Number)
				)
			{
				//*exists=1;
				//释放字符串
				if(d.type==dtString)
				{
					free(d.v.String);
				}
				return i;
			}
		
		}
	}
	return stackPush(&vm->constVector,&d);
}

//-----------------------------------------------------------------------------------
int xs_addConstString(_xsvm *vm,   char* name)
{
	_variable d;
	int len=strlen(name)+1;
	memset(&d,0,sizeof(d));
	d.type=dtString;
	d.v.String=(char*)calloc(1,len);
	strcpy(d.v.String,name);

	return xs_addConst(vm, d);
}
//-----------------------------------------------------------------------------------
int xs_objectExists(_xsvm *vm, unsigned short name, int *oi)
{
	int i;
	
	for(i=0; i<vm->staticObjects.count; i++)
	{
		_object *o=(_object*)stackIndex(&vm->staticObjects,i);
		if(o->name==name)
		{
			*oi=i;
			return XS_SUCCESS;
		}
	}
	return XS_FAILED;
}

//-----------------------------------------------------------------------------------
char* xs_getNewObjectName(_xsvm *vm)
{
	char *buf=(char*)calloc(16,1);
	char idx[16];
	memset(idx,0,16);
	strcpy(buf,"$o");
	vm->objectIndex++;
	strcat(buf,idx);
	return buf;
}

//-----------------------------------------------------------------------------------
int xs_newObject(_xsvm *vm, unsigned short name)
{
	int oi;
	char objName[32];
	if( xs_objectExists(vm,name,&oi)==XS_SUCCESS )
	{
		_object *o;
		_object no;//=malloc(sizeof(_object));
		int nname;
		_variable v;

		vm->objectIndex++;
		memset(objName,0,32);
		//strcpy(name,"$o");
		//strcat(name,ptemp);
		//itoa()
		//sprintf(objName,"$o%d",vm->objectIndex);
		nname=xs_addConstString(vm,objName);
		o=(_object*)listGet(&vm->dynamicObjects,oi);
		
		//noi=xs_addConst(buf);
		memcpy(&no,o,sizeof(_object));
		//copy new local variables
		//stackNew(&no.variables,sizeof(_variable),o->variables.count);
		//stackCopy(&no.variables,&o->variables);
		/*
		memcpy(no.variables->buff,o->variables.buff,o->variables.count*o->variables.typeSize);
		no.variables->count=o->variables.count;
		no.variables->maxCount=o->variables.maxCount;
		no.variables->typeSize=o->variables.typeSize;
		*/
		no.typeName=name;
		no.name=nname;
		
		listAdd(&vm->dynamicObjects,&no);

		v.v.Number=nname;
		v.type=dtObject;

		vm->globalStack[vm->stackCount]=v;
		vm->stackCount++;
		//stackPush(&vm->globalStack,&v);

		return XS_SUCCESS;
		//addCode(nname,Global,opPUSHOBJ);
	}
	else
	{
		return XS_EXCEPTION;
	}
	
}

//编译期
//-----------------------------------------------------------------------------------
int xs_addObject(_xsvm *vm,  char* name, xsbyte type)
{
	int oi;
	int index=xs_addVarName(vm,name);//xs_addConstString(name);
	if( xs_objectExists(vm,index,&oi)==XS_SUCCESS )
	{
		return oi;
	}
	else
	{
		_object o;
		memset(&o,0,sizeof(_object));
		o.name=index;
		o.typeName=index;
		o.parent=vm->currentObject;
		o.type=type;
		return stackPush(&vm->staticObjects,&o);
	}
}
//-----------------------------------------------------------------------------------
void xs_makeVarName(_xsvm *vm,   _object *owner, char *vn, char* newVN)
{
	_variable *v;
	v=(_variable*)stackIndex(&vm->lexer.varNameTable,owner->name);
	
	strcpy(newVN,v->v.String);
	strcat(newVN,".");
	strcat(newVN,vn);
}

//-----------------------------------------------------------------------------------
int xs_varExists(_object *o,unsigned short name, int *vi)
{
	int i;
	for(i=0; i<o->varCount; i++)
	{
		if(o->variables[i].name==name)
		{
			*vi=i;
			return XS_SUCCESS;
		}
	}
	return XS_FAILED;
}
//-----------------------------------------------------------------------------------
int xs_addVarName(_xsvm *vm,  char* name)
{
	_variable d;
	int len=strlen(name)+1;
	int i;	
	memset(&d,0,sizeof(d));
	d.type=dtString;
	d.v.String=(char*)calloc(1,len);
	strcpy(d.v.String,name);

	for(i=0; i<vm->lexer.varNameTable.count;i++)
	{
		_variable *dt=(_variable*)stackIndex(&vm->lexer.varNameTable,i);
		if(dt->type==d.type )
		{
			if( (d.type==dtString && strcmp(d.v.String,dt->v.String)==0) )
			{
				//*exists=1;
				//释放字符串
				free(d.v.String);
				return i;
			}
		
		}
	}
	return stackPush(&vm->lexer.varNameTable,&d);

}

int xs_addVar(_xsvm *vm, _object *o, char* name)
{
	int index=xs_addVarName(vm,name);//xs_addConstString(name);
	int vi;
	if( xs_varExists(o,index,&vi)==XS_SUCCESS )
	{
		return vi;
	}
	else
	{
		_variable d;
		if( o->varCount>=MAX_VARIABLE )
		{
			xs_throw(VE_MAX_VARIABLE);
		}

		memset(&d,0,sizeof(_variable));
		d.name=index;
		
		o->variables[o->varCount]=d;
		o->varCount++;

		return o->varCount-1;
	}
}
//-----------------------------------------------------------------------------------
int xs_getVarAddrByName(_xsvm *vm, _object *owner, char* name,int *scope, int *idx)
{
	int i,index;
	_object *o=owner;
	*scope=XSCURRENT;
	for(;;)
	{
		char newVN[maxVariableLength];
		xs_makeVarName(vm,o,name,newVN);
		index=xs_addVarName(vm,newVN);
		for( i=0; i<o->varCount; i++ )
		{
			_variable *v=&o->variables[i];
			if(v->name==index)
			{
				*idx=i;
				if( o==owner )
				{
					*scope=XSCURRENT;
				}
				else if(o==vm->globalObject)
				{
					*scope=XSGLOBAL;
				}
				return XS_SUCCESS;
			}
		}
		if(o->parent)
		{
			o=(_object*)o->parent;
		}
		else
		{
			break;
		}
	}
	return XS_EXCEPTION;
}
//-----------------------------------------------------------------------------------
char* xs_getVarNameByAddr(_xsvm *vm,  int index)
{
	if(index<vm->lexer.varNameTable.count && index>=0)
	{
		_variable *v=(_variable*)stackIndex(&vm->lexer.varNameTable,index);
		return v->v.String;

	}
	return NULL;
}




//-----------------------------------------------------------------------------------
void xs_convertToString(_xsvm *vm,_variable *v)
{
	switch(v->type)
	{
		case dtString:
		{
			return;
		}
		case dtNumber: 
		{
			int iv=v->v.Number;
			v->type=dtString;
			
			//gcvt(v->v.Number,7,v->v.String);			
			if(iv!=v->v.Number)
			{
				v->v.String=(char*)calloc(32,1);
				//sprintf(v->v.String,"%f",v->v.Number);
				//gcvt(v->v.Number,7,v->v.String);
				xs_gcvt(v->v.Number,7,v->v.String);
			}
			else
			{
				v->v.String=(char*)calloc(32,1);
				//itoa(iv,v->v.String,10);
				//sprintf(v->v.String,"%d",iv);
				xs_itoa(iv,v->v.String,10);
			}
			break;
		}
		case dtArray:
		{
			xs_freeArray(vm,v);
			v->type=dtString;
			v->v.String=(char*)calloc(16,1);
			strcpy(v->v.String,"[Array]");
			break;
		}
		case dtNull:
		{
			v->type=dtString;
			v->v.String=(char*)calloc(16,1);
			strcpy(v->v.String,"[Null]");
			break;
		}
		default:
		{
			v->type=dtString;
			v->v.String=(char*)calloc(16,1);
			strcpy(v->v.String,"[Object]");
			break;			
		}
	}
}

//-----------------------------------------------------------------------------------
char* xs_toString(_variable *v)
{
	int len=32;
	char *buf;
	if(v->type==dtString)
	{
		int len2=strlen(v->v.String)+1;
		len=len>len2?len:len2;
	}
	buf=(char*)calloc(len,1);
	switch(v->type)
	{
		case dtString:
		{
			strcpy(buf,v->v.String);
			break;
		}
		case dtNumber: 
		{
			//gcvt(v->v.Number,7,buf);
			int iv=v->v.Number;
			if(iv!=v->v.Number)
			{
				//gcvt(v->v.Number,7,buf);
				//sprintf(buf,"%f",v->v.Number);
				xs_gcvt(v->v.Number,7,buf);
								
			}
			else
			{
				//itoa(iv,buf,10);
				//sprintf(buf,"%d",iv);
				xs_itoa(iv,buf,10);
			}
			break;
		}
		case dtArray:
		{
			strcpy(buf,"[Array]");
			break;
		}
		case dtNull:
		{
			strcpy(buf,"[NULL]");
			break;
		}
		default:
		{
			strcpy(buf,"[Object]");
			break;			
		}
	}
	
	return buf;
}

//-----------------------------------------------------------------------------------
double xs_toNumber(_variable *v)
{
	switch(v->type)
	{
		case dtString:
		{
			char *ptr=NULL;
			double d=strtod(v->v.String,&ptr);
			if(*ptr)
			{
				d=0;
			}
			return d;
		}
		default:
		{
			return v->v.Number;
		}
	}
	
}






