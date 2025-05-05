#ifndef _XSQUEUE_h
#define _XSQUEUE_h

#include <stdio.h>
#include <stdlib.h>
#ifdef __cplusplus
extern "C" {
#endif

#if defined(_USERALIGN_)
#pragma pack(1)
#endif
	
typedef char ElementType;
struct QueueRecord
{
  int Capacity;
  int Front;
  int Rear;
  int Size;
  ElementType *Array;
};
#if defined(_USERALIGN_)
#pragma pack()
#endif

typedef struct QueueRecord *Queue;

Queue        queueCreate(int MaxElements);
void         queueFree(Queue Q);
void         queuePush( Queue Q, ElementType *e, int count );
ElementType* queueFront(Queue Q, int count) ;
void         queueRemove(Queue Q, int count) ;
ElementType* queuePop(Queue Q, int count) ;

int         queueIsEmpty(Queue Q);
int         queueIsFull(Queue Q);
void        queueMakeEmpty(Queue Q);

#ifdef __cplusplus
}
#endif

#endif  
  

