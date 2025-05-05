
#include "xsstack.h"

void stackResize(_stack *l, int count)
{
	if(count>=l->capacity)
	{
		int newCount=(count*11)/10+2;
		void *temp=calloc(1,newCount*l->typeSize);
		if(l->buff)
		{
			memcpy(temp,l->buff,l->count*l->typeSize);
			free( l->buff);
		}
		
		l->capacity=newCount;
		l->buff=(char*)temp;
	}
	
}
void stackNew(_stack* p,int typesize, int count)
{
	p->count=0;
	p->capacity=count+2;
	p->typeSize=typesize;
	p->buff=(char*)calloc(1,p->capacity*p->typeSize);
}

void stackFree(_stack *p)
{
	if( p->buff)
	{
		free( p->buff);
	}
}

int stackPush(_stack* l,void* v)
{
	if(l->count>=l->capacity)
	{
		int newCount=(l->count*11)/10+2;
		void *temp=calloc(1,newCount*l->typeSize);
		if(l->buff)
		{
			memcpy(temp,l->buff,l->count*l->typeSize);
			free( l->buff);
		}
		
		l->capacity=newCount;
		l->buff=(char*)temp;
	}

	//memcpy(l->buff[l->count],v,l->typeSize);
	memcpy( (l->buff + l->typeSize * l->count),v,l->typeSize);
	l->count++;

	return l->count-1;
}

void* stackGet(_stack *l)
{
	if(l->count>0)
	{
		//return l->buff[l->count-1];
		return (void*)(l->buff+l->typeSize*(l->count-1));
	}	
	return NULL;
}

void* stackIndex(_stack *l,int index)
{
	if( index>=0 && index<l->count)
	{
		return (void*)(l->buff+l->typeSize*index);
	}
	return NULL;
}

void* stackPop(_stack *l)
{
	if(l->count>0)
	{
		l->count--;
		//return l->buff[l->count];
		return l->buff+l->typeSize*l->count;
	}	
	return NULL;
}

void stackCopy(_stack *dst, _stack *src)
{
	dst->count=src->count;
	//dst->maxCount=src->maxCount;
	dst->typeSize=src->typeSize;

	memcpy(dst->buff,src->buff,src->typeSize*src->count);

}
