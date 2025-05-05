
#include "xsdef.h"
#include "xslexer.h"
#include "xsparser.h"
#include "xsvm.h"
#include "xsutil.h"

/*
parseStatement()
分析一行语句
只能以下列开始
1，函数调用f();
2，变量定义var f;
3，赋值语句f=0;
4，分支语句if{} switch(){}
5，循环语句do{}while() for(){}
6，函数定义function f();

*/

int parseStatements(_xsvm *vm)
{
	for(;;)
	{
		int rt=parseStatement(vm);
		if( rt==XS_SCOPECLOSE )
		{
			break;
		}
		else if( rt==XS_EXCEPTION )
		{
			return XS_EXCEPTION;
		}
	}
	return XS_SUCCESS;
}

int xs_includeFile(_xsvm *vm)
{
	//下一个必须为文件名
	char *fn;
	//char filename[256];
	char *scrtxt;
	//FILE *src;
	char *newscript;
	//int fsize;
	char *p=nextToken(vm);
	xsuint len;
	if( isString(p)!=XS_SUCCESS )
	{
		return XS_EXCEPTION;
	}			
	fn=toCString(p);
	
	//strcpy(filename, vm->workingPath );
	//strcat(filename, fn);
			
	//scrtxt=xs_readFile(filename);
	scrtxt=xs_extractFileData(vm->workingPath,fn, &len);
	free(fn);
	if(scrtxt)
	{
		int len=strlen(scrtxt)+strlen(vm->lexer.script)+10;
		newscript=(char*)calloc(len,1);
		strcpy(newscript, scrtxt);
		strcat(newscript, vm->lexer.currentPos);
		free(scrtxt);
		free(vm->lexer.script);
		vm->lexer.script=newscript;
		vm->lexer.currentPos=newscript;
	}
	else
	{
		return XS_EXCEPTION;
	}
	
	return XS_SUCCESS;
	
}
//一个statement必须以;或者}结束
//如果在for循环内，for(;;i++)，也可以)号结束
int parseStatement(_xsvm *vm)
{
	char *p=nextToken(vm);
    int rt=XS_SUCCESS;
	if(p)
	{
		if( isMatch(p,XSIF) )
			rt=parseIf(vm);
		else if( isMatch(p,XSSWITCH))
			rt=0;//parseSwitch(vm);
		else if( isMatch(p,XSWHILE) )
			rt=parseLoopWhile(vm);
		else if( isMatch(p,XSFOR) )
			rt=parseLoopFor(vm);
		else if( isMatch(p,XSFUNCTION) )
			rt=parseFunctionDefine(vm);
		else if( isMatch(p,XSCLASS) )
			rt=parseClassDefine(vm);
		else if( isMatch(p,XSVAR) )
			rt=parseVariableDefine(vm);
		else if(p[0]==';')
			return XS_SUCCESS;			
		else if( isMatch(p,XSINCLUDE) )
		{
			rt=xs_includeFile(vm);
		}
		//*/
		else if(strcmp(p,"}")==0)
		{
			//函数结束，返回上一层
			strcpy(vm->lexer.nextToken,p);
			return XS_SCOPECLOSE;
		}
		else if(isMatch(p,XSBREAK))
		{
			xs_addCode(vm,0,0,opBREAK);
			return XS_SUCCESS;
		}
		else if(isMatch(p,XSCONTINUE))
		{
			xs_addCode(vm, 0,0,opCONTINUE);
			return XS_SUCCESS;
		}
		else if(isMatch(p,XSRETURN))
		{	
			//return function
			//return; return a; return a+2+3;
			if( parseAssign(vm)!=XS_SUCCESS )
			{
				return XS_EXCEPTION;
			}
			//返回的结果在栈中，也可能没返回结果
			xs_addCode(vm, 0,0,opRET);
			return XS_SUCCESS;
		}
		else
		{
			//赋值 | 函数调用
			char ptemp[maxVariableLength];
			if( isVariable(p)!=XS_SUCCESS )
			{
				xs_throw(VE_INVALID_VARNAME);
			}
			
			//ptemp=malloc(strlen(p)+1);
			strcpy(ptemp,p);

			p=nextToken(vm);
			if(!p)
			{
				xs_throw(VE_END_OF_FILE);
			}

			if( strcmp(p,"=") ==0)
			{
				p=nextToken(vm);
				//对数组赋值
				if(strcmp(p,"{")==0)
				{
					if( parseArrayInitialize(vm, ptemp)==XS_EXCEPTION)
					{
						return XS_EXCEPTION;
					}
				}
				else 
				{
					strcpy(vm->lexer.nextToken,p);
					if( parseAssign(vm)==XS_EXCEPTION)
					{
						return XS_EXCEPTION;
					}
					else
					{
						int idx,scope;
						if( xs_getVarAddrByName(vm,vm->currentObject,ptemp,&scope,&idx)==XS_SUCCESS )
						{
		                    int added=0;
		                    if(!added)
		                    {
		                    	xs_addCode(vm, idx,scope,opASSIGN);
		                    }
						}
						else
						{
							return XS_EXCEPTION;
						}
		
					}
				}
			}
			else if( strcmp(p,"(")==0 )
			{
				//将参数PUSH入栈
				int paramCount=0;
				if( parseFunctionCallArgs(vm,&paramCount)==XS_SUCCESS )
				{
					char name[maxVariableLength];
					int index,oi;
					memset(name,0,maxVariableLength);
					strcpy(name,XSGLOBALOBJECT);
					strcat(name,ptemp);
					index=xs_addVarName(vm,name);//xs_addConstString(name);
					//free(ptemp);
					if( xs_objectExists(vm,index,&oi)==XS_SUCCESS )
					{
						_object *o=(_object*)stackIndex(&vm->staticObjects,oi);//listGet(&vm->objectVector,oi);
						//如果参数个数不匹配
						if(o->paramCount!=paramCount)
						{
							xs_throw(VE_INVALID_PARAMCOUNT);
						}

						xs_addCode(vm, oi,0,opCALL);
						//如果是独立的语句（非表达式/赋值计算），则恢复栈
						xs_addCode(vm, 0,0,opRESSTACK);
					}
					else
					{						
						if( parseCallAPI(vm,ptemp,paramCount)==XS_EXCEPTION )
						{
							return XS_EXCEPTION;
						}
						xs_addCode(vm, 0,0,opRESSTACK);
						//addCode(index,0,opCALLAPI);
					}
				}
				else
				{
					//free(ptemp);
					return XS_EXCEPTION;
				}
				//CALL FUNCTION
			}			
			else if(isMatch(p,XSINC))
			{
				int idx,scope;
				if( xs_getVarAddrByName(vm,vm->currentObject,ptemp,&scope,&idx)==XS_SUCCESS )
				{
					xs_addCode(vm, idx,scope,opINC);		
					p=nextToken(vm);
					if(strcmp(p,")")!=0 && strcmp(p,";")!=0)
					{
						strcpy(vm->lexer.nextToken,p);
					}
				}
				else
				{
					return XS_EXCEPTION;
				}

			}
			else if(isMatch(p,XSDEC))
			{
				int idx,scope;
				if( xs_getVarAddrByName(vm,vm->currentObject,ptemp,&scope,&idx)==XS_SUCCESS )
				{
					xs_addCode(vm, idx,scope,opDEC);
					p=nextToken(vm);
					if(strcmp(p,")")!=0 && strcmp(p,";")!=0)
					{
						strcpy(vm->lexer.nextToken,p);
					}
				}
				else
				{
					return XS_EXCEPTION;
				}
			}
			else if(strcmp(p,"[")==0)
			{
				//数组引用，此处必须为赋值语句
				//a[1]=0;a[x+y]="hello";
				int cc1,cc2,i;
				_stack tempStack;
				_stack *gc=&vm->gcode;
				if( vm->currentObject!=vm->globalObject)
				{
					gc=&vm->code;
				}
				
				cc1=gc->count;
				if(parseExpression(vm)==XS_SUCCESS)
				{
					int index,rt,scope;
					rt=xs_getVarAddrByName(vm,vm->currentObject,ptemp,&scope,&index);
					if(rt!=XS_SUCCESS)
					{
						xs_throw(VE_VAR_NOTDEFINED);
					}
					//push an array member
					xs_addCode(vm, index,scope,opASSIGNARR);
				}				
				cc2=gc->count;
				
				p=nextToken(vm);
				if(strcmp(p,"=")!=0)
				{
					return XS_EXCEPTION;
				}
								
				stackNew(&tempStack, sizeof(_instruction),10);
				cc1=cc2-cc1;
				for(i=0;i<cc1;i++)
				{
					stackPush(&tempStack, stackPop( gc ) );
				}
				if(parseExpression(vm)!=XS_SUCCESS)
				{
					stackFree(&tempStack);
					return XS_EXCEPTION;
				}
				
				for(i=0; i<cc1; i++ )
				{
					stackPush(gc,stackPop(&tempStack));
				}
				stackFree(&tempStack);
			}
			else
			{
				//free(ptemp);
				//raise error
				return XS_EXCEPTION;
			}
		}
		
	}
	else
	{
		return XS_SCOPECLOSE;
	}

	return rt;
}

int parseCallAPI(_xsvm *vm, char* apiName,int paramCount)
{
	int i;
	int count=vm->apiStack.count;
	_xsapi *api=(_xsapi *)vm->apiStack.buff;
	for(i=0; i<count; i++)
	{
		if(strcmp(api[i].apiName,apiName)==0)
		{
			if( paramCount!=api[i].paramCount )
			{
				xs_throw(VE_INVALID_PARAMCOUNT);
			}

			xs_addCode(vm,i,0,opCALLAPI);
			return XS_SUCCESS;
		}
	}
	
	xs_throw(VE_API_NOT_FOUND);
	return XS_EXCEPTION;
}


void handleSubOperation(_xsvm *vm)
{
	//处理减法,var a=b-c-d;
	_stack *gc=&vm->gcode;
	if( vm->currentObject!=vm->globalObject)
	{
		gc=&vm->code;
	}
					
	if( gc->count>1 )
	{
		_instruction *ci=(_instruction*)stackIndex(gc,gc->count-1);
		_instruction *pi=(_instruction*)stackIndex(gc,gc->count-2);
		if(ci->opCode==opSUB &&pi->opCode==opSUB)
		{
			pi->opCode=opADD;
		}
	}

}

/*
parseExpression(vm)
解析一个表达式
1，赋值语句的右边部分
2，函数调用参数部分
3，if,while判断语句括号内部分
*/
int parseExpression(_xsvm *vm)
{
    xsbyte lastOp = opNOP;

	_instruction opCodes[32];
    //byte opCodes[32];
	xsbyte opCodeCount=0;

	char *p=nextToken(vm);
	int index=0;
	int rt=XS_SUCCESS;
	double num;

    while(p)
    {
        xsbyte currOp=opNOP;
		int slen=strlen(p);
		int isbreak=0;
		if(slen==1)
		{
			switch(p[0])
			{
				case '+':
				{
					currOp = opADD;
					break;
				}
				case '-':
				{
					//如果之前为一个变量或数值，则应为减号，否则为负号
					if( isVariable(vm->lexer.preToken)==XS_SUCCESS || isString(vm->lexer.preToken)==XS_SUCCESS || isNumber(vm->lexer.preToken)==XS_SUCCESS 
							|| strcmp(vm->lexer.preToken,")")==0 || strcmp(vm->lexer.preToken,"]")==0 )
					{
						currOp = opSUB;
					}
					else
					{
						currOp=opNEG;
					}
					break;
				}
				case '*':
				{
					currOp = opMUL;
					break;
				}
				case '/':
				{
					currOp = opDIV;
					break;
				}
				case '%':
				{
					currOp=opMOD;
					break;
				}
				case '>':
				{
					currOp = opGT;
					break;
				}
				case '<':
				{
					currOp = opLT;
					break;
				}
				case '!':
				{
					currOp = opNOT;
					break;
				}
				case '(':
				{
					//递归取括号内表达式
					int r=parseExpression(vm);
					if( r!=XS_SUCCESS && r!=XS_RBRACKETS )
					{
						return XS_EXCEPTION;
					}
					break;
					//rt=LBRACKETS;
					//break;
				}
				case ')':
				{
					//右括号只可能在左括号后出现，所以在递归时处理，表示递归完毕返回
					rt=XS_RBRACKETS;
					isbreak=1;
					break;
				}				
				case ']':
				{
					isbreak=1;
					break;
				}
				case ';':
				{
					//遇到分号，表达式结束，返回
					//rt=SEMICOLON;
					isbreak=1;
					break;
				}
				case '}':
				{
					//对数组赋值var a={1,2,3};
					isbreak=1;
					rt=XS_SCOPECLOSE;
					//strcpy(vm->lexer.nextToken,p);
					break;
				}
				case ',':
				{
					//一般在函数调用内出现逗号，f(2,3,a+b);，或者在数组初始化时出现var arr={1,2,"hello",5.0};，逗号表明一个表达式的结束
					//rt=COMMA;
					isbreak=1;
					break;
				}
				default:
				{
					goto ENHANDLED;
				}
			}
			if( isbreak)
			{
				break;
			}
		}
		else if( slen==2)
		{
			if(isMatch(p,XSINC))
			{
				currOp=opINC;
			}
			else if(isMatch(p,XSDEC))
			{
				currOp=opDEC;
			}
			else if (isMatch(p,XSGE) || isMatch(p,XSGE2))
			{
				currOp = opGE;
			}
			else if (isMatch(p,XSLE) || isMatch(p,XSLE2))
			{
				currOp = opLE;
			}
			else if (isMatch(p,XSEQ))
			{
				currOp = opEQ;
			}
			else if(isMatch(p,XSNE))
			{
				currOp=opNE;
			}
			else if(isMatch(p,XSOR))
			{
				currOp=opOR;
			}
			else if(isMatch(p,XSAND))
			{
				currOp=opAND;
			}
			else if(isMatch(p,XSCONC))
			{
				currOp=opCONC;
			}
			else
			{
				goto ENHANDLED;
			}
		}
        else
		{
ENHANDLED:
            //rs += "Push \t" + ci + "\r\n";
            //是否为值，数字、布尔、字串，都保存在静态数据表中了
			if(isMatch(p,XSNEW))
			{
				//new object
				if( parseNewClass(vm)!=XS_SUCCESS)
				{
					return XS_EXCEPTION;
				}
			}
			else if(isVariable(p)==XS_SUCCESS)
			{
				//f, f(), f.f1.f2, f[0]
				//是否函数调用
				//char *ptemp=malloc(strlen(p)+1);
				char ptemp[maxVariableLength];
				strcpy(ptemp,p);

				p=nextToken(vm);
				if(p && strcmp(p,"(")==0 )
				{
					int paramCount=0;
					if( parseFunctionCallArgs(vm, &paramCount)==XS_SUCCESS )
					{
						//FUNCTION CALL
						char name[maxVariableLength];
						int index,oi;
						memset(name,0,maxVariableLength);
						strcpy(name,XSGLOBALOBJECT);
						strcat(name,ptemp);
						//sprintf(name,"$G.%s",ptemp);
						index=xs_addVarName(vm,name);//xs_addConstString(name);
						if( xs_objectExists(vm,index,&oi)==XS_SUCCESS )
						{
							_object *o=(_object*)stackIndex(&vm->staticObjects,oi);//listGet(&vm->objectVector,oi);
							//如果参数个数不匹配
							if(o->paramCount!=paramCount)
							{
								xs_throw(VE_INVALID_PARAMCOUNT);
							}
							xs_addCode(vm, oi,0,opCALL);
						}
						else
						{
							if( parseCallAPI(vm, ptemp,paramCount)==XS_EXCEPTION )
							{
								return XS_EXCEPTION;
							}
							//addCode(0,index,0,opCALLAPI);
							//free(ptemp);
						}
					}
				}
				else if(p && strcmp(p,"[")==0 )
				{
					//数组引用
					//a[1],a[x+y],
					//若内部表达式失败,则会xs_throw
					if(parseExpression(vm)==XS_SUCCESS)
					{
						int index,rt,scope;
						rt=xs_getVarAddrByName(vm,vm->currentObject,ptemp,&scope,&index);
						if(rt!=XS_SUCCESS)
						{
							xs_throw(VE_VAR_NOTDEFINED);
						}
						//push an array member
						xs_addCode(vm, index,scope,opPUSHARR);

						//多维数组
						/*
						p=nextToken(vm);
						while(p)
						{
							if( strcmp(p,"[")==0 )
							{
								if(parseExpression(vm)==XS_SUCCESS)
								{
									xs_addCode(0,0,opPUSHMARR);
									p=nextToken(vm);
								}
								else
								{
									return XS_EXCEPTION;
								}
							}
							else
							{
								//strcpy( vm->lexer.nextToken,p);
								break;
							}							
						}
						continue;
						*/
					}
				}
				else
				{
					int index,rt,scope;
					rt=xs_getVarAddrByName(vm,vm->currentObject,ptemp,&scope,&index);
					if(rt!=XS_SUCCESS)
					{		
						xs_throw(VE_VAR_NOTDEFINED);
					}

					xs_addCode(vm, index,scope,opPUSH);
					
					continue;
				}

			}
			else if(toNumber(p,&num)==XS_SUCCESS)
			{
				_variable v;
				int index;
				v.type=dtNumber;
				v.v.Number=num;
				index=xs_addConst(vm,v);

				xs_addCode(vm, index,XSCONST,opPUSH);
			}
			else if(p[0]=='\"' && p[strlen(p)-1]=='\"' && strlen(p)>2 )
			{
				int count=strlen(p)-1;
				char *buf=(char*)calloc(count+1,1);
				int i;
				int di=0;
				//转义符
				for(i=1; i<count; i++ )
				{
					if(p[i]=='\\')
					{
						i++;
						switch(p[i])
						{
							case 't':
							{
								buf[di]='\t';
								break;
							}
							case 'n':
							{
								buf[di]='\n';
								break;
							}
							case 'r':
							{
								buf[di]='\r';
								break;
							}
							case '\"':
							{
								buf[di]='\"';
								break;
							}
							case '\\':
							{
								buf[di]='\\';
								break;
							}
							default:
							{
								buf[di]=p[i];
							}
						}
						di++;
						continue;
					}
					
					buf[di]=p[i];
					di++;
				}

				i=xs_addConstString(vm,buf);
				free(buf);
				xs_addCode(vm,i,XSCONST,opPUSH);
			}
			else
			{
				return XS_EXCEPTION;
			}
			
            //ParseValue();

            //是否为函数调用 f()

            //是否为对象成员引用 o.id.id...

            //是否为数组元素 t[0]
            
        }

        if (currOp < lastOp && currOp!=opNOP )
        {
            while ( opCodeCount>0 )
            {
                int i = opCodeCount - 1;
				if (currOp < opCodes[i].opCode)
                {
                    //rs += GetOpString((_opCode)opCodes[i]) + "\r\n";
                	xs_addCode(vm, 0,0,opCodes[i].opCode);
                	handleSubOperation(vm);
                    opCodeCount--;
                }
                else
                {
                    break;
                }

            }

            lastOp = opNOP;
            if (opCodeCount > 0)
            {
                int i = opCodeCount - 1;
				lastOp = opCodes[i].opCode;
            }

            if (currOp < lastOp)
            {
                //rs += GetOpString(currOp) + " \r\n";
            	xs_addCode(vm, 0,0,currOp);
            	handleSubOperation(vm);
            }
            else
            {
				opCodes[opCodeCount].opCode=currOp;
				//opCodes[opCodeCount].address=currAddr;

				opCodeCount++;
                lastOp = currOp;
            }
        }
        else if(currOp!=opNOP)
        {
			opCodes[opCodeCount].opCode=currOp;
			opCodeCount++;
            lastOp = currOp;
        }

		p=nextToken(vm);
		
    }

	for (index = (opCodeCount - 1); index >= 0; index--)
    {
        //rs += GetOpString((_opCode)opCodes[i])+"\r\n";
    	xs_addCode(vm, 0,0,opCodes[index].opCode);
    	handleSubOperation(vm);
    }
    //return rs;
	
	return rt;

}



int parseFunctionDefine(_xsvm *vm)
{
	char np[maxVariableLength];
	int index;
	_object *o;//=malloc(sizeof(_object));
	char *p=nextToken(vm);
	if(isVariable(p)!=XS_SUCCESS)
	{
		//raise error
		xs_throw(VE_INVALID_VARNAME);
	}
	
	//create new function object
	xs_makeVarName(vm,vm->currentObject,p,np);
	//addFunction(np);
	index=xs_addObject(vm,np,dtFunction);
	o=(_object*)stackIndex(&vm->staticObjects,index);//
	
	if(strcmp(np,XSONUPDATE)==0)
		{
		vm->onUpdate=o;
		}
	else if(strcmp(np,XSONEVENT)==0)
		{
		vm->onEvent=o;
		}
	vm->currentObject=o;
	
	//分析参数
	if( parseFunctionArgs(vm)!=XS_SUCCESS)
	{
		xs_throw(VE_INVALID_ARGS);
	}
	
	//参数个数
	vm->currentObject->paramCount=vm->currentObject->varCount;

	p=nextToken(vm);

	//必须以"{"开头
	if(strcmp(p,"{")!=0)
	{
		//error
		xs_throw(VE_MISSING_LEFTBRACE);
	}

	if( parseBody(vm)!=XS_SUCCESS)
	{
		return XS_EXCEPTION;
	}

	//返回上一层
	if( vm->currentObject->parent!=NULL )
	{
		vm->currentObject=(_object*)vm->currentObject->parent;
		//free(o);
	}

	return XS_SUCCESS;
}

int parseFunctionArgs(_xsvm *vm)
{
	//()
	//(var i,var j)
	char *p=nextToken(vm);
	char newVN[maxVariableLength];
    //int addr;
	if(p[0]!='(')
	{
		xs_throw(VE_MISSING_LBRACKETS);
	}
	
	for(;;)
	{
		p=nextToken(vm);
		if(p[0]==')')
		{
			return XS_SUCCESS;
		}
		else if(isMatch(p,XSVAR))
		{
			p=nextToken(vm);
			if( isVariable(p)!=XS_SUCCESS )
			{
				xs_throw(VE_INVALID_VARNAME);
			}
			xs_makeVarName(vm,vm->currentObject, p, newVN);

			xs_addVar(vm,vm->currentObject,newVN);

			p=nextToken(vm);
			if(p[0]==',')
			{
				//multiple args
				continue;
			}
			else if(p[0]==')')
			{
				break;
			}
			else
			{
				return XS_EXCEPTION;
			}
		}
		else
		{
			return XS_EXCEPTION;
		}
	}
	
	return XS_SUCCESS;
}

int parseFunctionCallArgs(_xsvm *vm, int *paramCount)
{
	//f(); a.f();
	//a=a+f();
	//a=a+f(1,2,3);
	//a=a+f(f(),f()+2);
	//变量名后面接左括号，表明是函数调用，本函数分析左括号右边的
	int codeCount2,codeCount,rt;
	//int LBrackets=1;
	//int RBrackets=0;
	*paramCount=0;
	for(;;)
	{
		_stack *gc=&vm->gcode;
		if( vm->currentObject!=vm->globalObject)
		{
			gc=&vm->code;
		}
						
		codeCount=gc->count;
		rt=parseExpression(vm);
		
		codeCount2=gc->count;
		if(codeCount2<=codeCount )
		{
			break;
		}
		else
		{
			*paramCount=*paramCount+1;
		}
		/*
		if( rt==RBRACKETS )
		{
			RBrackets++;
			//return XS_EXCEPTION;
		}
		else if( rt==LBRACKETS )
		{
			LBrackets++;
			//return XS_EXCEPTION;
		}
		else 
		*/
		if( rt==XS_RBRACKETS )
		{
			break;
		}
		else if( rt!=XS_SUCCESS )
		{
			return XS_EXCEPTION;
		}
		
	}
	//先给所有的参数赋值，然后调用函数，需要找到函数对应的object


	return XS_SUCCESS;

}

int parseClassDefine(_xsvm *vm)
{
	//class C{}
	char *p1;
	char np[maxVariableLength];
	int index;
	_object *o;//=malloc(sizeof(_object));
	char *p=nextToken(vm);
	if(isVariable(p)!=XS_SUCCESS)
	{
		//raise error
		return XS_EXCEPTION;
	}
	
	//create new function object
	xs_makeVarName(vm,vm->currentObject,p,np);
	//addFunction(np);
	index=xs_addObject(vm,np,dtClass);
	o=(_object*)stackIndex(&vm->staticObjects,index);

	vm->currentObject=o;

	p1=nextToken(vm);

	//必须以"{"开头
	if(strcmp(p1,"{")!=0)
	{
		//error
		return XS_EXCEPTION;
	}

	if( parseBody(vm)!=XS_SUCCESS)
	{
		return XS_EXCEPTION;
	}

	//返回上一层
	if( vm->currentObject->parent!=NULL )
	{
		vm->currentObject=(_object*)vm->currentObject->parent;
		//free(o);
	}

	return XS_SUCCESS;
}

//opNew
//instruction的 address为要new的对象的名称地址，在const表中
int parseNewClass(_xsvm *vm)
{
	//当前并非真正的可以new object,所以当作函数处理,new的动作在函数内完成
	return parseExpression(vm);
}

//var a;
//var a=expr;
//var a=new A();
int parseVariableDefine(_xsvm *vm)
{
	//下一个token必须为变量名
	char *vn=nextToken(vm);
	char *p;
	char newVN[maxVariableLength];
	int addr;
	if( isVariable(vn)!=XS_SUCCESS )
	{
		//raise error
		#ifdef xs_debug
		//lastError="Invalid variable name:"+vn;
		#endif
		xs_throw(VE_INVALID_VARNAME);
	}
	//create variable
	//添加变量名到常量表，返回在常量表中的地址
	//添加变量定义到当前Object变量表，返回地址
	xs_makeVarName(vm,vm->currentObject, vn, newVN);

	addr=xs_addVar(vm,vm->currentObject,newVN);//xs_addVarName(newVN);
	strcpy(newVN,vn);
	/*
	变量定义操作不产生代码
	_instruction ins;
	
	xs_addCode(vm->currentFunction,ins);
	*/
	//如果有赋值语句（其它为；号结束）
	p=nextToken(vm);
	if(strcmp(p,"=")==0)
	{
		//_stackDateItem sd;
		//sd.address=addr;
		//分析赋值语句
		//_instruction ins;
		p=nextToken(vm);
		if(strcmp(p,"{")==0)
		{
			if( parseArrayInitialize(vm, newVN)==XS_EXCEPTION)
			{
				return XS_EXCEPTION;
			}
		}		
		else 
		{
			strcpy(vm->lexer.nextToken,p);
			if( parseAssign(vm)!=XS_SUCCESS )
			{
				return XS_EXCEPTION;
		
			}
			xs_addCode(vm, addr,XSCURRENT,opASSIGN);
		}
		//ins.address=addr;
		//ins.opCode=opASSIGN;
		
	}
	else if(strcmp(vn,";")!=0)
	{
		//错误
		xs_throw(VE_MISSING_SEMICOLON);
	}
	return XS_SUCCESS;
}

int parseArrayInitialize(_xsvm *vm, char *ptemp)
{
	//{0,2,"hello"};
	int arrindex=0;
	int index,rt,scope;
	rt=xs_getVarAddrByName(vm,vm->currentObject,ptemp,&scope,&index);
	if(rt!=XS_SUCCESS)
	{
		xs_throw(VE_VAR_NOTDEFINED);
	}
	rt=parseExpression(vm);
	while(rt!=XS_EXCEPTION)
	{
		_variable v;
		int idx;
		v.type=dtNumber;
		v.v.Number=arrindex;
		idx=xs_addConst(vm,v);

		xs_addCode(vm,idx,XSCONST,opPUSH);
		xs_addCode(vm,index,scope,opASSIGNARR);
		arrindex++;		
		if(rt==XS_SCOPECLOSE) break;
		rt=parseExpression(vm);
	}
	return XS_SUCCESS;
}
int parseAssign(_xsvm *vm)
{
	//表达式，函数调用，NEW语句等
	//赋值语句必须以分号结束
	return parseExpression(vm);
}

int parseLoopFor(_xsvm *vm)
{
	//for([expr];[expr];[expr]){}
	char *p=nextToken(vm);
	int loopBegin,loopEnd,i,cc1,cc2;
	_stack tempStack;
	_stack *gc=&vm->gcode;
	stackNew(&tempStack, sizeof(_instruction),10);

	if(strcmp(p,"(")!=0 )
	{
		stackFree(&tempStack);
		xs_throw(VE_MISSING_LBRACKETS);
	}

	//3 exprs
	//for循环第一项，为一个普通表达式
	if( parseStatement(vm)==XS_EXCEPTION )
	{
		stackFree(&tempStack);
		return XS_EXCEPTION;
	}
	
	if( vm->currentObject!=vm->globalObject)
	{
		gc=&vm->code;
	}
					
	//添加一个标签,for循环的开始
	loopBegin=gc->count;//xs_addCode(vm->currentObject->labelIndex,0,opLOOP);
	//vm->currentObject->labelIndex++;

	//第二项必须为空或一个表达式
	if( parseExpression(vm)==XS_EXCEPTION )
	{
		stackFree(&tempStack);
		return XS_EXCEPTION;
	}
	//判断第二项，跳转jump
	//xs_addCode(0,0,0,opTEST);
	//xs_addCode(0,0,0,opBREAK);
    xs_addCode(vm,0,0,opJNZ_TEMP);

	//第三项为空或表达式，指令放在for循环的最后面执行
	cc1=gc->count;
	if( parseStatement(vm)==XS_EXCEPTION )
	{
		stackFree(&tempStack);
		return XS_EXCEPTION;
	}
	cc2=gc->count;
	cc1=cc2-cc1;
	for(i=0;i<cc1;i++)
	{
		stackPush(&tempStack, stackPop( gc ) );
	}

	//如果为{
	p=nextToken(vm);
	if(p && strcmp(p,"{")==0)
	{
		//for(){}
		if( parseBody(vm)==XS_EXCEPTION )
		{
			stackFree(&tempStack);
			return XS_EXCEPTION;
		}
	}
	else
	{
		//for() i++;
		strcpy(vm->lexer.nextToken,p);
		if( parseStatement(vm)==XS_EXCEPTION )
		{
			stackFree(&tempStack);
			return XS_EXCEPTION;
		}
	}

	//第三段指令
	for(i=0; i<cc1; i++ )
	{
		stackPush(gc,stackPop(&tempStack));
	}
	stackFree(&tempStack);

	//for循环结束
	xs_addCode(vm,0,0,opCONTINUE);//循环
	loopEnd=xs_addCode(vm,0,0,opLOOPEND);

	//替换掉标签
	for( i=loopBegin; i<gc->count; i++ )
	{
		_instruction *ins=(_instruction*)stackIndex(gc,i);
        if( ins->opCode==opJNZ_TEMP )
        {
             //跳转到结束
            ins->opCode=opJNZ;
			ins->address=loopEnd;
			if(ins->address2>0)
			{
				ins->opCode=ins->address2;
			}
        }
		else if(ins->opCode==opBREAK)
		{
			//跳转到结束
			ins->opCode=opJMP;
			ins->address=loopEnd;
		}
		else if(ins->opCode==opCONTINUE)
		{
			//跳转到开始
			ins->opCode=opJMP;
			ins->address=loopBegin;
		}
	}

	return XS_SUCCESS;
}

int parseLoopWhile(_xsvm *vm)
{
	//while(expr){}
	char *p=nextToken(vm);
	int loopBegin,loopEnd,i,cc1,cc2;
	_stack *gc=&vm->gcode;
	if(vm->globalObject!=vm->currentObject)
	{
		gc=&vm->code;
	}
	
	if(strcmp(p,"(")!=0 )
	{
		xs_throw(VE_MISSING_LBRACKETS);
	}
	loopBegin=gc->count;//xs_addCode(0,0,opLOOP);
	cc1=gc->count;
	if( parseExpression(vm)==XS_EXCEPTION )
	{
		return XS_EXCEPTION;
	}
	cc2=gc->count;
	if(cc2<=cc1)
	{
		return XS_EXCEPTION;
	}

	//xs_addCode(0,0,0,opTEST);
	//xs_addCode(0,0,0,opBREAK);
    xs_addCode(vm,0,0,opJNZ_TEMP);

	//必须为{
	p=nextToken(vm);
	if(strcmp(p,"{")!=0)
	{
		xs_throw(VE_MISSING_LEFTBRACE);
	}

	if( parseBody(vm)==XS_EXCEPTION )
	{
		return XS_EXCEPTION;
	}
	
	//while循环结束
	xs_addCode(vm,0,0,opCONTINUE);//循环
	loopEnd=xs_addCode(vm,0,0,opLOOPEND);

	//替换掉标签
	for( i=loopBegin; i<gc->count; i++ )
	{
		_instruction *ins=(_instruction*)stackIndex(gc,i);
        if( ins->opCode==opJNZ_TEMP )
        {
             //跳转到结束
            ins->opCode=opJNZ;
			ins->address=loopEnd;
        }
		else if(ins->opCode==opBREAK)
		{
			//跳转到结束
			ins->opCode=opJMP;
			ins->address=loopEnd;
		}
		else if(ins->opCode==opCONTINUE)
		{
			//跳转到开始
			ins->opCode=opJMP;
			ins->address=loopBegin;
		}
	}
	return XS_SUCCESS;

}
/*
int parseSwitch(_xsvm *vm)
{
	return XS_EXCEPTION;

}
*/
int parseBody(_xsvm *vm)
{
	//{ body }
	char *p;
	if( parseStatements(vm)==XS_EXCEPTION )
	{
		return XS_EXCEPTION;
	}
	//必须以}结束
	p=nextToken(vm);
	if(strcmp(p,"}")!=0)
	{
		xs_throw(VE_MISSING_RBRACE);
	}
	
	return XS_SUCCESS;
}

int optimizeJNZ(_xsvm *vm)
{
	_stack *gc=&vm->gcode;
	_instruction *ins;
	int c;
	int opCode=0;
	if(vm->globalObject!=vm->currentObject)
	{
		gc=&vm->code;
	}

    ins=(_instruction *)gc->buff;
    c=gc->count;
    opCode=0;
    if( c<1 ) return -1;
    switch( ins[c-1].opCode )
    {
        case opGT: opCode=opJG;break;
        case opGE: opCode=opJGE;break;
        case opLT: opCode=opJL;break;
        case opLE: opCode=opJLE;break;
        case opEQ: opCode=opJEQ;break;
        case opNE: opCode=opJNE;break;  
    }

    if( opCode>0 )
    {
        gc->count--;
        vm->currentObject->codeCount--;
        
        return xs_addCode(vm,0,0,opCode);
    }
    return -1;
}

int parseIf(_xsvm *vm )
{
	char *p=nextToken(vm);
	int i,end,begin,insi;
	_instruction *ins;
	_stack *gc=&vm->gcode;
	if(strcmp(p,"(")!=0)
	{
		xs_throw(VE_MISSING_LBRACKETS);
	}
	
	//第一个if语句
	if(parseExpression(vm)==XS_EXCEPTION )
	{
		return XS_EXCEPTION;
	}
	//begin=xs_addCode(0,0,0,opTEST);
	//insi=xs_addCode(0,0,0,opJMP);
    insi=optimizeJNZ(vm);
    if( insi==-1 )
    {
        insi=xs_addCode(vm,0,0,opJNZ);
    }
	begin=insi;
	//如果为{
	p=nextToken(vm);
	if(strcmp(p,"{")==0)
	{		
		if( parseBody(vm)==XS_EXCEPTION )
		{
			return XS_EXCEPTION;
		}
	}
	else
	{
		//if() i++;
		strcpy(vm->lexer.nextToken,p);
		if( parseStatement(vm)==XS_EXCEPTION )
		{
			return XS_EXCEPTION;
		}
	}
	//跳转到最后
	xs_addCode(vm,0,0,opIFEND);
	
	if(vm->globalObject!=vm->currentObject)
	{
		gc=&vm->code;
	}

	
	end=gc->count;//xs_addCode(0,0,opNOP);
	ins=(_instruction*)stackIndex(gc,insi);
	ins->address=end;
	//ins=listGet(vm->code,insi);
	//ins->address=end;

	//第一个if语句结束，跳转到IF ELSE段的结尾
	//xs_addCode(0,0,opIFELSEEND);
	for(;;)
	{
		p=nextToken(vm);
		if(!p)
		{
			break;
		}

		if(isMatch(p,XSELSEIF) )
		{
			p=nextToken(vm);
			if(strcmp(p,"(")!=0)
			{
				xs_throw(VE_MISSING_LBRACKETS);
			}
						
			if(parseExpression(vm)==XS_EXCEPTION )
			{
				return XS_EXCEPTION;
			}
			
			//ins->address=xs_addCode(0,0,0,opTEST);
			//insi=xs_addCode(0,0,0,opJMP);
            insi=optimizeJNZ(vm);
            if( insi==-1 )
            {
                insi=xs_addCode(vm,0,0,opJNZ);
            }
			
			//必须为{
			p=nextToken(vm);
			if(strcmp(p,"{")==0)
			{				
				if( parseBody(vm)==XS_EXCEPTION )
				{
					return XS_EXCEPTION;
				}			
			}
			else
			{
				//elseif() i++;
				strcpy(vm->lexer.nextToken,p);
				if( parseStatement(vm)==XS_EXCEPTION )
				{
					return XS_EXCEPTION;
				}
			}
			xs_addCode(vm,0,0,opIFEND);
			end=gc->count;//xs_addCode(0,0,opNOP);

			ins=(_instruction*)stackIndex(gc,insi);
			ins->address=end;
		}
		else if(isMatch(p,XSELSE))
		{
			//end of if else
			//必须为{
			p=nextToken(vm);
			if(strcmp(p,"{")==0)
			{				
				if( parseBody(vm)==XS_EXCEPTION )
				{
					return XS_EXCEPTION;
				}	
			}
			else
			{
				//else i++;
				strcpy(vm->lexer.nextToken,p);
				if( parseStatement(vm)==XS_EXCEPTION )
				{
					return XS_EXCEPTION;
				}
			}
			end=gc->count; //xs_addCode(0,0,opNOP);
			break;
		}
		else
		{
			strcpy(vm->lexer.nextToken,p);
			break;
		}
	}
	//替换掉标签
	for( i=begin; i<gc->count; i++ )
	{
		_instruction *ins=(_instruction*)stackIndex(gc,i);
		if(ins->opCode==opIFEND)
		{
			//跳转到结束
			ins->opCode=opJMP;
			ins->address=end;
		}
	}

	return XS_SUCCESS;
}

/*
void parseValue(_xsvm *vm)
{
	return;
}
*/
int isVariable(char *p)
{
	if( strlen(p)<1 || strlen(p)>64 )
	{
		return XS_EXCEPTION;
	}

	if( (p[0]>='a' && p[0]<='z')||(p[0]>='A' && p[0]<='Z') || p[0]=='_')
	{
		return XS_SUCCESS;
	}
	return XS_EXCEPTION;
}

int toNumber(char* p, double *rt)
{
	char *ptr=NULL;
	*rt=strtod(p,&ptr);
	if(*ptr)
	{
		return XS_EXCEPTION;
	}
	return XS_SUCCESS;
}
int isNumber(char* p)
{
	char *ptr=NULL;
	strtod(p,&ptr);
	if(*ptr)
	{
		return XS_EXCEPTION;
	}
	return XS_SUCCESS;
}

int isString(char *p)
{
	if(p && p[0]=='"' && p[strlen(p)-1]=='"' )
	{
		return XS_SUCCESS;
	}

	return XS_FAILED;
}

char *toCString(char *p)
{
	int count=strlen(p)-1;
	char *buf=(char*)calloc(count+1,1);
	int i;
	int di=0;
	//转义符
	for(i=1; i<count; i++ )
	{
		if(p[i]=='\\')
		{
			i++;
			switch(p[i])
			{
				case 't':
				{
					buf[di]='\t';
					break;
				}
				case 'n':
				{
					buf[di]='\n';
					break;
				}
				case 'r':
				{
					buf[di]='\r';
					break;
				}
				case '\"':
				{
					buf[di]='\"';
					break;
				}
				case '\\':
				{
					buf[di]='\\';
					break;
				}
				default:
				{
					buf[di]=p[i];
				}
			}
			di++;
			continue;
		}
		
		buf[di]=p[i];
		di++;
	}
	return buf;
}
