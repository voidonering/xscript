#ifndef _XSLIST_H
#define _XSLIST_H
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
	void** buff;
	int count;
	int maxCount;
	int typeSize;
}_list;

//#define LNEW(T,count)((T*)(listNew(sizeof(T),count)))

void listResize(_list* t);
void listNew(_list* p,int typesize, int count);
void listCopy(_list *dst,_list *src);

void listFree(_list *p);

int listAdd(_list *l,void *v);
void listRemove(_list *l, int i);

void* listGet(_list *l, int i);

//void listModify(_list *l, int i, void* v);
#if defined(_USERALIGN_)
#pragma pack()
#endif

#endif
