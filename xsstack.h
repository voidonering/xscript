#ifndef _XSSTACK_H
#define _XSSTACK_H
#include <stdlib.h>
#include <string.h>
#include "stdio.h"

#if defined(_USERALIGN_)
#if defined(__WINS__)
#define PACKED
#else
#define PACKED __attribute__((packed, aligned(1)))
#endif
	
#pragma pack(1) 
#endif

typedef struct 
{
	char* buff;
	int count;
	int capacity;
	int typeSize;
}_stack;

#if defined(_USERALIGN_)
#pragma pack()
#endif

void stackResize(_stack* t, int count);
void stackNew(_stack* p,int typesize, int count);
void stackFree(_stack *p);
int stackPush(_stack *l,void *v);
void* stackPop(_stack *l);
void* stackGet(_stack *l);
void* stackIndex(_stack *l,int index);
void stackCopy(_stack *dst, _stack *src);

#endif
