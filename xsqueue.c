#include "xsqueue.h"
#include "stdlib.h"
#include "stdio.h"
#include "xsdef.h"

//max length=10M
#define MAX_QUEUE_SIZE	(1024*1024*10)

int queueIsEmpty(Queue Q) 
{
  return Q->Size == 0;
}

int queueIsFull(Queue Q) 
{
  return Q->Size == Q->Capacity;
}

Queue queueCreate(int MaxElements) 
{
  Queue Q;

  Q = (Queue)malloc (sizeof(struct QueueRecord));
  if (Q == NULL) 
  {
    	return NULL;
  }

  Q->Array = (ElementType *)malloc( sizeof(ElementType) * MaxElements );
  if (Q->Array == NULL) 
  {
	  return NULL;
  }

  Q->Capacity = MaxElements;
  queueMakeEmpty(Q);

  return Q;
}

void queueMakeEmpty(Queue Q) 
{

  Q->Size = 0;
  Q->Front = 1;
  Q->Rear = 0;

}

void queueFree(Queue Q) 
{
  if (Q != NULL) 
  {
    free(Q->Array);
    free(Q);
  }
}

int Succ(int Value, Queue Q) 
{
  if (++Value == Q->Capacity) 
  {
    Value = 0;
  }
  return Value;
}

void queueResize(Queue Q, int newSize)
{
	if(newSize<=Q->Capacity)
	{
		return;
	}
	else
	{
		int len= (newSize*11)/10;
		ElementType *e=(ElementType *)malloc(sizeof(ElementType)*len);
		ElementType *oe=(ElementType *)queueFront(Q, Q->Size);
		memcpy(e,oe,Q->Size*sizeof(ElementType));
		free(Q->Array);
		free(oe);
		Q->Array=e;
		Q->Capacity =len ;
	}
}

void queuePush(Queue Q, ElementType *e, int count ) 
{
	if( (Q->Size+count)>Q->Capacity )
	{
		if( (Q->Size+count)>MAX_QUEUE_SIZE )
		{
			return;
		}
		else
		{
			queueResize(Q, (Q->Size+count));
			queuePush( Q, e, count);
		}
	}
	else
	{
		int i;	
		for( i=0; i<count; i++)
		{
			Q->Size++;
			Q->Rear = Succ(Q->Rear, Q);
			Q->Array[Q->Rear] = e[i];
		}
	}
}

//get the front element from the queue, do not remove it
ElementType* queueFront(Queue Q, int count) 
{
	  if (Q->Size<count) 
	  {
		  //error
		  return NULL;
	  } 
	  else 
	  {
		  ElementType *e=(ElementType *)malloc(sizeof(ElementType)*count);
		  int ofront=Q->Front;
		  int i;
		  for(i=0; i<count; i++)
		  {
			  e[i]=Q->Array[Q->Front];
			  Q->Front = Succ(Q->Front, Q);
		  }
		  Q->Front=ofront;
		  return e;
	  }

}

//just remove the front element from the queue
void queueRemove(Queue Q, int count) 
{
	int i;
	count=((count>Q->Size)?Q->Size:count);
	for( i=0; i<count; i++)
	{
		Q->Size--;
		Q->Front = Succ(Q->Front, Q);
	}
}

//remove and return the front element from the queue
ElementType* queuePop(Queue Q, int count) 
{
  if (Q->Size<count) 
  {
	  //error
	  return NULL;
  } 
  else 
  {
	  ElementType *e=(ElementType *)malloc(sizeof(ElementType)*count);
	  int i;
	  for( i=0; i<count; i++)
	  {
		  Q->Size--;
		  e[i]=Q->Array[Q->Front];
		  Q->Front = Succ(Q->Front, Q);
	  }
	  return e;
  }
}
