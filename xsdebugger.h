
#ifndef _XSDEBUGGER_H
#define _XSDEBUGGER_H
#include "xsdef.h"

#ifdef XS_DEBUG
#include "SocketEngine.h"
#include "e32std.h"
#include "xsqueue.h"

#pragma pack(1)
/*
#define dxs_init xs_init
#define dxs_free xs_free
#define dxs_compile xs_compile
#define dxs_decompile xs_decompile
#define dxs_deCompiletoFile xs_deCompileToFile
*/

//protocol
enum _xs_debug_datatypes
{
	//执行脚本
    DT_CS_RUN,
    DT_SC_RUN,
    
    //停止脚本
    DT_CS_STOP,
    DT_SC_STOP,

    //暂停脚本
    DT_CS_PAUSE,
    DT_SC_PAUSE,

    //继续脚本
    DT_CS_RESUME,
    DT_SC_RESUME,

    //单步模式
    DT_CS_STEPMODE,
    DT_SC_STEPMODE,

    //脚本源程序
    DT_CS_GETSOURCECODE,
    DT_SC_GETSOURCECODE,

    //脚本指令
    DT_CS_GETINSTRUCTIONS,
    DT_SC_GETINSTRUCTIONS,

    //局部变量表
    DT_CS_GETLOCALVARIABLES,
    DT_SC_GETLOCALVARIABLES,

    //全局变量
    DT_CS_GETGLOBALVARIABLES,
    DT_SC_GETGLOBALVARIABLES,	
	
    //设置断点
    DT_CS_SETBREAKPOINT,
    DT_SC_SETBREAKPOINT,

    //获取断点
    DT_CS_GETBREAKPOINTS,
    DT_SC_GETBREAKPOINTS,
    
    //当前执行位置,在源码和指令表中
    DT_SC_CURRENTPOSITION,

    //进入语句
    DT_CS_STEPINTO,
    DT_SC_STEPINTO,

    //逐语句
    DT_CS_STEPOVER,
    DT_SC_STEPOVER,
    
    //获得二进制代码
    DT_CS_GETBINARYCODE,
    DT_SC_GETBINARYCODE,
        
    //发送消息
    DT_CS_INFORMATION,
    DT_SC_INFORMATION,
    
    //compile
    DT_CS_COMPILE,
};


class xsDebugger;

class socketObserver:public xsSocketEngineObserver
{
public:
    void SocketState( TInt aNewState ) ;  
    void SocketData( xsushort DataType, char *data, int len ) ;
    void *appData;
	
	socketObserver(void *app)
	{
		appData=app;
	}
};

class xsDebugger// : public CActive
{
public:
	CSocketEngine *iSocketEngine ;
	socketObserver *so;
	_xsvm *vm;
	RThread mainThread;
	RThread socketThread;
	/*
	RThread runThread;
	RThread	onEventThread;
	RThread	onUpdateThread;
	RThread currentThread;
	RThread socketThread;
	void RunL();
	void DoCancel();
	
	void Start();
*/
	//int stepmode;
	//int paused;
	//int stopped;
	
	//是否正在运行,global,onEvent,onUpdate不可交叉执行,同一时间只能执行某一个
	//int running;
	
	xsDebugger(_xsvm *_vm);
	void setBusyMode( int v);
	
	void dxs_stop();
	void dxs_pause();
	void dxs_resume();
	int dxs_run( );
	void dxs_onEvent();
	void dxs_onUpdate();
	void dxs_stepMode();
	void dxs_getSourceCode();
	void dxs_getInstructions();
	void sendBuffer(xsushort DataType, char *buf, int alen);
	void dxs_getGlobalVariables();
	void dxs_getLocalVariables();
	void dxs_setBreakPoint(char *data, int len);
	void dxs_getBreakPoint();
	void dxs_sendPosition(int sourcePosition, int instructionPosition);
	void dxs_stepInto( );
	void dxs_stepOver();
	void dxs_getBinaryCode();
	void dxs_sendInformation(char *info);
	
	void connect(char* ip, int port);
};


void dxs_run(_xsvm *vm );
void dxs_onEvent(_xsvm *vm);
void dxs_onUpdate(_xsvm *vm);
xsDebugger *dxs_InitDebugger(_xsvm *vm);
void dxs_freeDebugger(_xsvm *vm);
void dxs_suspend();

#endif

#endif



