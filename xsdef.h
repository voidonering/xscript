
/*
xs.h
定义一些通用的结构
*/
/*

*/
#ifndef	_XSDEF_H
#define _XSDEF_H

#include "xsconfig.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "xslist.h"
#include "xsstack.h"
#include "setjmp.h"
#include "xserror.h"
#include "xshash.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_USERALIGN_)
#if defined(__WINS__)
#define PACKED
#else
#define PACKED __attribute__((packed, aligned(1)))
#endif
	
#pragma pack(1) 
#endif

#ifndef xsbyte
#define xsbyte unsigned char
#endif
#ifndef xsushort
#define xsushort unsigned short
#endif

#define xsuint unsigned long

//异常处理
extern jmp_buf Jump_Buffer;
#define xs_try    if(!setjmp(Jump_Buffer))
#define xs_catch else
#define xs_throw(eid) handleException(vm, eid)


#define DEFVARS _variable *vars[4];vars[0]=(_variable*)(vm->constVector.buff);vars[1]=vm->currentObject->variables;vars[2]=vm->globalObject->variables;vars[3]=vm->globalStack;
#define VAR(s,i) (vars[s][i])

#define xsfree(x) if(x){free(x);x=NULL;}
#define XS_OBJECT unsigned int

enum _xsEvent
{
	XE_KEY_DOWN,
	XE_KEY_UP,
	XE_KEY_PRESSED,
	
	XE_MOUSE_CLICK,
	XE_MOUSE_DOWN,
	XE_MOUSE_UP,
	XE_MOUSE_MOVE,
	
};

enum _xsKey
{
	//mouse
	XS_VK_LBUTTON,
	XS_VK_RBUTTON,
	XS_VK_MBUTTON,
	
	//key
	XS_VK_SPACE,
	XS_VK_ENTER,	//XS_VK_FIRE
	XS_VK_LSOFT,
	XS_VK_RSOFT,
	XS_VK_UP,
	XS_VK_DOWN,
	XS_VK_LEFT,
	XS_VK_RIGHT,
	
	XS_VK_0,
	XS_VK_1,
	XS_VK_2,
	XS_VK_3,
	XS_VK_4,
	XS_VK_5,
	XS_VK_6,
	XS_VK_7,
	XS_VK_8,
	XS_VK_9,

	XS_VK_A,
	XS_VK_B,
	XS_VK_C,
	XS_VK_D,
	XS_VK_E,
	XS_VK_F,
	XS_VK_G,
	XS_VK_H,

};

enum _type
{
	dtNull,
	dtBoolean,
	dtNumber,
	dtString,
	dtObject,
	dtFunction,
	dtClass,
	dtArray,
};

enum _dataScope
{
	XSCONST,
	XSCURRENT,
	XSGLOBAL,
	XSSTACK,
	/*
    Local,
	Const,
	Stack,
	*/
};

enum _opCode
{
    opNOP,    
	opAND,		//&& a and b
	opOR,		//|| a or b	
	opCONC,		//连接
	opGT,       //1 大于>greater than 
    opGE,       //2 大于等于>=greater or equal to
    opLT,       //3 小于<
    opLE,       //4 小于等于<=less or equal to
    opEQ,       //5 相等equal
	opNE,		//6	不相等
	opADD,      //7 加+
    opSUB,      // 减-
    opMUL,      //8 乘*
    opDIV,      //9 除/
	opMOD,		//10 求余%    
	opNOT,      //11 求反!
	opNEG,		//	取负
	opTEST,		//从栈中弹出一个值，若为真，则忽略下一指令(指令+1)
	opRET,		//return from function
	opLOOP,		//loop begin
	opLOOPEND,	//loop end
	opASSIGN,	//12 赋值	
	opINC,		//自加
	opDEC,		//自减
	opPUSH,		//13 压入栈
	opPOP,		//14 弹出栈
	opCALL,		//call function
	opCALLAPI,	//call api
	opJMP,		//JUMP to address, no condition 无条件跳转
    opJNZ,      //JUMP when stack value is ZERO
    opJG,       //1 大于时不跳转>greater than
    opJGE,       //2 大于等于时不跳转>=greater or equal to
    opJL,       //3 小于时不跳转<
    opJLE,       //4 小于等于时不跳转<=less or equal to
    opJEQ,       //5 相等时不跳转equal
	opJNE,		//6	不相等时不跳转
	opNEW,		//create object	
	opPUSHARR,	//将数组元素压入栈
	opASSIGNARR,//对数组元素赋值
	opPUSHMARR,	//多维数组	
	opLAB,		//label
	opBREAK,	//break;
	opCONTINUE,	//continue
    opJNZ_TEMP,    
	opIFEND,
	opRESSTACK,

};

enum _commonValue
{
	XS_SUCCESS,
	XS_FAILED,
	XS_EXCEPTION,
	XS_RETURN,			//	return
	XS_LBRACKETS,		//	(
	XS_RBRACKETS,		//	)
	XS_SEMICOLON,		//	;
	XS_COMMA,			//	,
	XS_SCOPECLOSE,
};

typedef union 
{
	//char	Char;
	//short	Short;
	//int		Int;
	//float	Float;
	XS_NUMBER	Number;
	//void*	Object;	//如果是Object，则将ObjectName放在int值内
	char*	String;
}_value;

typedef struct 
{
     _value	v;
	 xsushort type;//该数据的类型，为_Type中的一个
	 xsushort name;   //变量的名字，字符串，在常量表中的地址
}_variable;

typedef struct
{
	xsushort address;//max 256*4 global vars/local vars，单个函数最多1024条指令
	xsbyte scope:4;//4，为_DataScope中的一个
	xsbyte layer:4;//16，变量所在的层parent->parent->parent
}_address;
//一个可执行指令，4字节
typedef struct 
{
	xsbyte opCode;        //6bits, max 64 opcodes
	xsbyte scope;//4，为_DataScope中的一个
	//byte layer:4;//16，变量所在的层parent->parent->parent
	xsushort address;//max 256*4 global vars/local vars，单个函数最多1024条指令
}_instruction2;

typedef struct 
{
	xsbyte opCode;        //6bits, max 64 opcodes
	xsushort address;	//MAX *256
	xsushort address2;
	xsbyte scope:4;	//0:CONST, 1:CURRENT, 2:GLOBAL, 3-8: 
	xsbyte scope2:4;

#ifdef XS_DEBUG
	xsushort	lineNumber;
	xsushort	breakPoint;
#endif

}_instruction;


//for debugging, do not use multi-thread, use CActive to debug in symbian
typedef struct
{
	//起始指令
	_instruction *beg;
	//当前指令
	_instruction *ins;
	//结束指令
	_instruction *end;
	//记录堆栈位置以便弹出
	int stackCount;
	//该段代码的执行次数,也即一个函数的调用次数
	int runCount;
}_stepState;

//每个function都是一个object
typedef struct 
{
	xsushort name;	//address in const table，实例名
	xsushort typeName;	//类型名
	xsbyte paramCount;		//参数数量
	//byte returnValue;		//是否返回值
	xsbyte type;			//类型，dtObject(create by classes),dtFunction
	//byte refCount;		//应用数量，为0则删除
	//ushort labelIndex;
	void* parent;
	//_stack variables;
	xsushort varCount;
	//_stack code;
	xsushort codeStart;
	xsushort codeCount;
	
	//_stepState state;
	_variable variables[MAX_VARIABLE];
	
}_object;

/*
typedef struct
{
	int count;
	_variable *vars;
}_array;
*/


typedef struct
{
	//source script
	char*	script;
	int srcLen;
	int currentLine;
	char* currentPos;
	int tempTokenLen;
	char currentToken[maxVariableLength];
	char tempToken[maxVariableLength];
	char preToken[maxVariableLength];
	char nextToken[maxVariableLength];
	_stack varNameTable;
}_lexer;

//#ifdef XS_DEBUG
/*
#include "e32std.h"
#include "SIPExSocketEngine.h"
class xsDebugger;
*/
typedef void (*debugapi)(void *vm);
typedef void (*sendpositionapi)(void *vm, int sourcePosition, int insPosition);
typedef struct
{
	//is paused
	int		paused;
	//is stopped
	int		stopped;
	
	//is step mode
	int		stepmode;
	
	//是否正在运行,global,onEvent,onUpdate不可交叉执行,同一时间只能执行某一个
	int isBusy;
	//onupdate
	//int		update;
	//event
	//int		event;
	
	//line number in source code
	int 	currentSourceLine;
	//line number in instructions
	int		currentInstructionLine;
	
	void*	debugger;
	
	//suspend the debugging thread
	debugapi suspend;
	//debugapi wait;
	
	sendpositionapi sendPosition;
	//debugging threads
	/*
	RThread	runThread;
	RThread	onEventThread;
	RThread	onUpdateThread;
	RThread currentThread;
	*/
	//debugging client socket
	//socketObserver *so;
	
}_xsdebugger;
//#endif


typedef struct 
{
	//函数表 function table
	//_list* functionVector;
	
	//全局对象 global object(script application)
	_object* globalObject;
	//当前对象 current object/function
	_object* currentObject;
	
	//callback function, call by main application
	_object* onUpdate;
	
	//callback function, calling when an event occur
	_object* onEvent;
	//_function* currentFunction;

	//calculating stack count
    int stackCount;

	//object index
	int objectIndex;

	//最后一次错误消息 last error message id
	int lastError;

	//xs lexer
	_lexer lexer;
	
	//常量表，保存数字、字串等值 consts
	_stack constVector;
	
	//动态创建的对象表 dynamic created objects list
	_list dynamicObjects;
	
	//static objects
	_stack staticObjects;

	//外部API栈 api stack
	_stack apiStack;

	//运算栈 calculate stack
	_variable globalStack[maxStackCount];

	//对象引用 object reference
	struct hashtable* gch;
	//_list objectReference;
	
	//global instructions
	_stack gcode;
	
	//all instructions
	_stack code;
	
	//debugger
	#ifdef XS_DEBUG
	_xsdebugger debugger;

	#endif
	
	//_stepState *state;
	//store app data
	xsuint	appData;
	
	
	//工作目录 working directory
	char workingPath[256];


}_xsvm;
//extern _xsvm vm;

typedef void (*xsapi)(_xsvm *vm);
typedef void (*disposeapi)(xsuint object);

typedef struct
{
	xsuint	object;
	xsuint	refCount;
	disposeapi	dispose;
}_objectReference;


typedef struct
{
	xsapi api;
	int paramCount;
	char apiName[maxAPINameLength];
}_xsapi;


void handleException(_xsvm *vm, int eid);

#if defined(_USERALIGN_)
#pragma pack()
#endif

#ifdef __cplusplus
}
#endif
#endif
