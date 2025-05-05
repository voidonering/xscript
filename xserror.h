
#ifndef _XSERROR_H
#define _XSERROR_H

#ifdef __cplusplus
extern "C" {
#endif

enum _vmErrors
{
	VE_SUCCESS,

	//编译时错误errors in compiling time
	VE_API_APINAMETOOLONG,		//API名字太长
	VE_API_NOT_FOUND,

	VE_INVALID_PARAMCOUNT,		//非法参数个数

	VE_VAR_NOTDEFINED,			//变量未定义
	VE_INVALID_VARNAME,			//不合法的函数或变量名，必须以字母_开头
	VE_INVALID_ARGS,			//不合法的函数参数
	VE_MISSING_LEFTBRACE,		//缺少左大括号 {
	VE_MISSING_RBRACE,			// }
	VE_MISSING_LBRACKETS,		//缺少左括号（
	VE_MISSING_SEMICOLON,		// ;

	VE_END_OF_FILE,				//非法文件结束

	//运行时错误run time error
	VE_STACK_OVERFLOW,			//堆栈溢出
	VE_STACK_ERROR,				//栈出错(变量数量不足)
	VE_STACK_NOTNUMBER,			//需要数字，但不是
	VE_STACK_NOTSTRING,			//需要字串
	VE_STACK_NOTOBJECT,
	
	VE_INVALID_LAYER,			//变量对象所在层不正确
	VE_DIV_ZERO,				//除0
	VE_INVALID_NUMBER,			//
	
	VE_INVALID_ARRAY,			//非法数组

	VE_MAX_VARIABLE,			//单个函数，或全局变量超过MAX_VARIABLE（默认128）
	VE_MAX_FUNCTION_INSTRUCTION,		//单个函数最大指令数，默认1字节256
	
};

char* getErrorString(int errorCode);


#ifdef __cplusplus
}
#endif
#endif
