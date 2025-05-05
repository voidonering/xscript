#ifndef _XSCONFIG_h
#define _XSCONFIG_h

#ifdef __cplusplus
extern "C" {
#endif
	
//#define XS_DEBUG_GC
//#define XS_DEBUG

//最长API函数名字
#define maxAPINameLength 128
//最长变量名
#define maxVariableLength 256
//堆栈最大值
#define maxStackCount 256
	
//单个函数最多变量数，最多常量数
#define MAX_VARIABLE 256
//单个函数最多指令数
#define MAX_FUNCTION_INSTRUCTION 65535

//XS Numbers的定义,若定义为int,可能会稍微提高速度
#define XS_NUMBER double

//字符串长度无限制,动态创建
	
#ifdef __cplusplus
}
#endif

#endif  	
